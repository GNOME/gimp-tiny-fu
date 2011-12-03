## Process this file with automake to produce Makefile.in

if OS_WIN32
mwindows = -mwindows
no_undefined = -no-undefined
endif

noinst_LIBRARIES = libftx.a

AM_CFLAGS = \
    -DSTANDALONE=0     \
    -DUSE_INTERFACE=1  \
    $(GLIB_CFLAGS)

libftx_a_SOURCES = ftx.c

EXTRA_DIST = \
    LICENSE           \
    README            \
    ftx-functions.txt \
    listhome.scm
