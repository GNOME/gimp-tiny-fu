## Process this file with automake to produce Makefile.in

if OS_WIN32
mwindows = -mwindows
WINSOCK_LIBS = -lws2_32
no_undefined = -no-undefined
else
libm = -lm
endif

libtinyscheme=-L../tinyscheme -ltinyscheme $(libm)
libftx=-L../ftx -lftx


AM_CFLAGS = \
    -DSTANDALONE=0     \
    -DUSE_INTERFACE=1  \
    $(GTK_CFLAGS)      \
    $(GIMP_CFLAGS)     \
    $(mwindows)

LDADD = \
    $(libtinyscheme)       \
    $(libftx)              \
    $(GTK_LIBS)            \
    $(GIMP_LIBS)           \
    $(GIMPUI_LIBS)         \
    $(WINSOCK_LIBS)

libexecdir = $(GIMP_LIBDIR)/tiny-fu

libexec_PROGRAMS = tiny-fu

tiny_fu_SOURCES = \
    tiny-fu.c              \
    tiny-fu-console.c      \
    tiny-fu-console.h      \
    tiny-fu-enums.h        \
    tiny-fu-eval.c         \
    tiny-fu-eval.h         \
    tiny-fu-interface.c    \
    tiny-fu-interface.h    \
    tiny-fu-intl.h         \
    tiny-fu-regex.c        \
    tiny-fu-regex.h        \
    tiny-fu-script.c       \
    tiny-fu-script.h       \
    tiny-fu-scripts.c      \
    tiny-fu-scripts.h      \
    tiny-fu-server.c       \
    tiny-fu-server.h       \
    tiny-fu-text-console.c \
    tiny-fu-text-console.h \
    tiny-fu-types.h        \
    tiny-fu-utils.c        \
    tiny-fu-utils.h        \
    scheme-wrapper.c       \
    scheme-wrapper.h
