#ifndef MTK_BINDER_PLUGIN_H
#define MTK_BINDER_PLUGIN_H

#include <radio_registry.h>
#include <radio_instance.h>

#define DEV							"/dev/hwbinder"

#define MTK_RADIO_IFACE_PREFIX		"vendor.mediatek.hardware.radio@"
#define MTK_RADIO_IFACE(x)			MTK_RADIO_IFACE_PREFIX "2.1::" x
#define MTK_RADIO					MTK_RADIO_IFACE("IRadio")
#define MTK_RADIO_RESPONSE			MTK_RADIO_IFACE("IRadioResponse")
#define MTK_RADIO_INDICATION		MTK_RADIO_IFACE("IRadioIndication")

typedef struct radio_instance_priv RadioInstancePriv;

struct radio_instance_priv {
    GUtilIdlePool* idle;
    GBinderClient* client;
    GBinderRemoteObject* remote;
    GBinderLocalObject* response;
    GBinderLocalObject* indication;
    GHashTable* resp_quarks;
    GHashTable* ind_quarks;
    gulong death_id;
    char* dev;
    char* slot;
    char* key;
};

#endif
