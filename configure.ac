AC_INIT([mtk-binder-plugin], [0,1])
AC_CONFIG_SRCDIR(src/mtk-binder-plugin.c)
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_ENABLE_SHARED
AC_DISABLE_STATIC

AC_SUBST(GBR_LIBS)
AC_SUBST(GBR_CFLAGS)

AC_SUBST(BDR_LIBS)
AC_SUBST(BDR_CFLAGS)

if pkg-config libgbinder-radio; then
    GBR_LDFLAGS=`pkg-config --libs libgbinder-radio`
    GBR_CFLAGS=`pkg-config --cflags libgbinder-radio`
else
    AC_MSG_ERROR(Get gbinder-radio library from https://github.com/mer-hybris/libgbinder-radio)
fi


if pkg-config libgbinder; then
    BDR_LDFLAGS=`pkg-config --libs libgbinder`
    BDR_CFLAGS=`pkg-config --cflags libgbinder`
else
    AC_MSG_ERROR(Get gbinder library from https://github.com/mer-hybris/libgbinder)
fi

libdir=/usr/lib/ofono/plugins

AC_OUTPUT(Makefile src/Makefile)
