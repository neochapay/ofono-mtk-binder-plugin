#define OFONO_API_SUBJECT_TO_CHANGE
#include <ofono/plugin.h>
#include <ofono/log.h>

#include <gbinder.h>

#include <gutil_idlepool.h>
#include <gutil_log.h>
#include <gutil_misc.h>

#include <glib-object.h>

#include "mtk-binder-plugin.h"

static GBinderLocalReply* radio_instance_indication(GBinderLocalObject* obj, GBinderRemoteRequest* req, guint code, guint flags, int* status, void* user_data) {
	RadioInstance* self = RADIO_INSTANCE(user_data);
    const char* iface = gbinder_remote_request_interface(req);

	ofono_info("GO TO radio_instance_indication");
	
    if (!g_strcmp0(iface, MTK_RADIO_INDICATION)) {
        GBinderReader reader;
        guint type;

        /* All these should be one-way */
        GASSERT(flags & GBINDER_TX_FLAG_ONEWAY);
        gbinder_remote_request_init_reader(req, &reader);
        if (gbinder_reader_read_uint32(&reader, &type) &&
            (type == RADIO_IND_UNSOLICITED || type == RADIO_IND_ACK_EXP)) {
            //GQuark quark = radio_instance_ind_quark(self, code);
            gboolean handled = FALSE;

            if (type == RADIO_IND_ACK_EXP && !handled) {
                ofono_info("ack unhandled indication");
                radio_instance_ack(self);
            }
            
            *status = GBINDER_STATUS_OK;
        } else {
            ofono_info("Failed to decode indication %u", code);
            *status = GBINDER_STATUS_FAILED;
        }
    } else {
        ofono_info("Unexpected indication %s %u", iface, code);
        *status = GBINDER_STATUS_FAILED;
    }

	return NULL;
}

static GBinderLocalReply* radio_instance_response(GBinderLocalObject* obj, GBinderRemoteRequest* req, guint code, guint flags, int* status, void* user_data) {
	
	ofono_info("GO TO radio_instance_response");
	
    RadioInstance* self = RADIO_INSTANCE(user_data);
    const char* iface = gbinder_remote_request_interface(req);

    if (!g_strcmp0(iface, MTK_RADIO_RESPONSE)) {
        /* All these should be one-way transactions */
        GASSERT(flags & GBINDER_TX_FLAG_ONEWAY);
        if (code == RADIO_RESP_ACKNOWLEDGE_REQUEST) {
            ofono_info(MTK_RADIO_RESPONSE " %u acknowledgeRequest", code);
        } else {
            /* All other responses have RadioResponseInfo */
            GBinderReader reader;
            GBinderBuffer* buf;

            gbinder_remote_request_init_reader(req, &reader);
            buf = gbinder_reader_read_buffer(&reader);
            GASSERT(buf && buf->size == sizeof(RadioResponseInfo));
            if (buf && buf->size == sizeof(RadioResponseInfo)) {
                const RadioResponseInfo* info = buf->data;
                gboolean handled = FALSE;

                if (info->type == RADIO_RESP_SOLICITED_ACK_EXP && !handled) {
                    ofono_info("ack unhandled response");
                    radio_instance_ack(self);
                }
            }
            gbinder_buffer_free(buf);
        }
        *status = GBINDER_STATUS_OK;
    } else {
        ofono_info("Unexpected response %s %u", iface, code);
        *status = GBINDER_STATUS_FAILED;
    }
    return NULL;
}

static void radio_instance_died(GBinderRemoteObject* obj, void* user_data) {
}

static void mtk_radio_connect(const char* iface, const char* slot, const char* key) {
	ofono_info("GO TO mtk_radio_connect");
	
	RadioInstance* self = NULL;
	GBinderServiceManager* sm = gbinder_servicemanager_new(DEV);
	
	if (sm) {
		int status = 0;
		
		char* fqname = g_strconcat(iface, "/", slot, NULL);
        GBinderRemoteObject* remote = gbinder_servicemanager_get_service_sync(sm, fqname, &status);
		
		if(remote) {
			RadioInstancePriv* priv;
			GBinderLocalRequest* req;
			GBinderRemoteReply* reply;
			GBinderWriter writer;
			
			ofono_info("Connected to %s", fqname);
			
			gbinder_remote_object_ref(remote);
			
			self = g_object_new(RADIO_TYPE_INSTANCE, NULL);
			priv = self->priv;
			self->slot = priv->slot = g_strdup(slot);
			self->dev = priv->dev = g_strdup(DEV);
			self->key = priv->key = g_strdup(key);
			
			priv->remote = remote;
			priv->client = gbinder_client_new(remote, iface);
			priv->indication = gbinder_servicemanager_new_local_object
				(sm, MTK_RADIO_INDICATION, radio_instance_indication, self);
			priv->response = gbinder_servicemanager_new_local_object
				(sm, MTK_RADIO_RESPONSE, radio_instance_response, self);
			priv->death_id = gbinder_remote_object_add_death_handler
				(remote, radio_instance_died, self);
				
			/* IRadio::setResponseFunctions */
			req = gbinder_client_new_request(priv->client);
			gbinder_local_request_init_writer(req, &writer);
			gbinder_writer_append_local_object(&writer, priv->response);
			gbinder_writer_append_local_object(&writer, priv->indication);
			reply = gbinder_client_transact_sync_reply(priv->client,RADIO_REQ_SET_RESPONSE_FUNCTIONS, req, &status);
			ofono_info("setResponseFunctions %s status %d", slot, status);
			gbinder_local_request_unref(req);
			gbinder_remote_reply_unref(reply);

			ofono_info("Instance '%s'", slot);
			
			 gutil_idle_pool_add_object(priv->idle, g_object_ref(sm));
		} else {
			ofono_info("mtk_radio_connect: Remote not found");
		}
		gbinder_servicemanager_unref(sm);
		g_free(fqname);
	} else {
		ofono_info("mtk_radio_connect: sm not found");
	}
}

static int mtk_binder_plugin_init()
{
	ofono_info("Initializing MTK binder plugin.");

	mtk_radio_connect(MTK_RADIO,"slot1","");
	mtk_radio_connect(MTK_RADIO,"slot2","");

    return 0;
}

static void mtk_binder_plugin_exit()
{
    DBG("Bye bye MTK");
}

OFONO_PLUGIN_DEFINE(mtk_binder, "MTK binder plugin",
    OFONO_VERSION, OFONO_PLUGIN_PRIORITY_DEFAULT,
    mtk_binder_plugin_init, mtk_binder_plugin_exit)
