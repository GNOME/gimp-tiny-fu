## Process this file with automake to produce Makefile.in

AM_CFLAGS = \
    -DSTANDALONE=1      \
    -DUSE_INTERFACE=1   \
    -DUSE_MATH=1        \
    -DUSE_ASCII_NAMES=0 \
    -DUSE_STRLWR=0      \
    -DUSE_DL=1          \
    -DSUN_DL=1          \
    $(GLIB_CFLAGS)        

libexecdir = $(GIMP_LIBDIR)/tiny-fu

libexec_PROGRAMS = tinyscheme

tinyscheme_SOURCES = \
    dynload.c        \
    scheme.c         \
    opdefines.h      \
    scheme-private.h \
    scheme.h

LDADD = -ldl -lm

EXTRA_DIST = \
    BUILDING   \
    CHANGES    \
    COPYING    \
    hack.txt   \
    Manual.txt \
    MiniSCHEMETribute.txt
