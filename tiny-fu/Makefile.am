## Process this file with automake to produce Makefile.in

libtinyscheme=$(top_builddir)/tinyscheme/libtinyscheme.a
libftx=$(top_builddir)/ftx/libftx.a
libre=$(top_builddir)/re/libre.a

if OS_WIN32
mwindows = -mwindows
WINSOCK_LIBS = -lwsock32
endif

AM_CFLAGS = \
    -DUSE_INTERFACE=1  \
    -DUSE_STRCASECMP=1 \
    -DUSE_STRLWR=1     \
    -I$(top_srcdir)    \
    $(GIMP_CFLAGS)

AM_LDFLAGS = $(mwindows)

libexecdir = $(GIMP_LIBDIR)/plug-ins

libexec_PROGRAMS = tiny-fu

tiny_fu_SOURCES = \
    gimp-constants.h       \
    tiny-fu-types.h        \
    tiny-fu-enums.h        \
    tiny-fu-console.c      \
    tiny-fu-console.h      \
    tiny-fu-interface.c    \
    tiny-fu-interface.h    \
    tiny-fu-text-console.h \
    tiny-fu-text-console.c \
    tiny-fu-intl.h         \
    tiny-fu-scripts.c      \
    tiny-fu-scripts.h      \
    tiny-fu-server.c       \
    tiny-fu-server.h       \
    tiny-fu.c              \
    ts-wrapper.c           \
    ts-wrapper.h

LDADD = \
    $(libtinyscheme) \
    $(libftx)        \
    $(libre)         \
    $(GIMP_LIBS)     \
    $(WINSOCK_LIBS)  \
    $(INTLLIBS)
