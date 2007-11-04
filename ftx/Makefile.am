## Process this file with automake to produce Makefile.in

if OS_WIN32
mwindows = -mwindows
no_undefined = -no-undefined
endif

AM_CFLAGS = \
    -DUSE_INTERFACE=1 \
    -I$(top_srcdir)   \
    $(GIMP_CFLAGS)

ftx_ladir = $(GIMP_LIBDIR)/tiny-fu

ftx_la_LTLIBRARIES = ftx.la

ftx_la_SOURCES = ftx.c ftx.h

ftx_la_LDFLAGS = \
    -module          \
    -avoid-version   \
    $(no_undefined)  \
    $(mwindows)

EXTRA_DIST = \
    LICENSE           \
    README            \
    ftx-functions.txt \
    listhome.scm
