## Process this file with automake to produce Makefile.in

if OS_WIN32
mwindows = -mwindows
no_undefined = -no-undefined
endif
    
AM_CFLAGS = \
    -DSTANDALONE=1    \
    -DUSE_INTERFACE=1 \
    -I$(top_srcdir)   \
    $(GIMP_CFLAGS)

re_ladir = $(GIMP_LIBDIR)/tiny-fu

re_la_LTLIBRARIES = re.la

re_la_SOURCES = \
    cclass.h    \
    cname.h     \
    debug.c     \
    debug.ih    \
    re.c        \
    re.h        \
    regcomp.c   \
    regcomp.ih  \
    regerror.c  \
    regerror.ih \
    regex.h     \
    regex2.h    \
    regexec.c   \
    regfree.c   \
    split.c     \
    split.h     \
    utils.h

re_la_LDFLAGS = \
    -module          \
    -avoid-version   \
    $(no_undefined)  \
    $(mwindows)

#engine.c and engine.ih are listed below rather than above since the
#file is #include'ed in the source files and not compiled separately.
EXTRA_DIST = \
    COPYRIGHT   \
    README      \
    README.1st  \
    WHATSNEW    \
    engine.c    \
    engine.ih   \
    mkh         \
    regex.001   \
    regex.3     \
    regex.7     \
    re.makefile \
    re.scm
