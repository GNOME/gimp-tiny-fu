## Process this file with automake to produce Makefile.in

SUBDIRS = \
	po         \
	ftx        \
	tinyscheme \
	tiny-fu    \
	scripts

if PLATFORM_WIN32
no_undefined = -no-undefined
path_separator = ;
else
path_separator = :
endif

tinyfubase = tiny-fu

tinyfudir = $(GIMP_LIBDIR)/$(tinyfubase)

tinyfuenvdir = $(GIMP_LIBDIR)/environ
tinyfuinterpdir = $(GIMP_LIBDIR)/interpreters
TINYFU_BIN = $(tinyfudir)/tiny-fu

tinyfuenvfile = $(tinyfuenvdir)/tiny-fu.env
tinyfuinterpfile = $(tinyfuinterpdir)/tiny-fu.interp

install-env-file:
	$(mkinstalldirs) '$(DESTDIR)$(tinyfuenvdir)'
	echo '$(path_separator) TINYFUPATH=$${gimp_plug_in_dir}/$(tinyfubase)' > '$(DESTDIR)$(tinyfuenvfile)'
	echo '$(path_separator) TINYSCHEMEINIT=$${gimp_plug_in_dir}/$(tinyfubase)/script-fu.init' >> '$(DESTDIR)$(tinyfuenvfile)'

install-interp-file:
	$(mkinstalldirs) '$(DESTDIR)$(tinyfuinterpdir)'
	echo 'tiny-fu=$(TINYFU_BIN)' > '$(DESTDIR)$(tinyfuinterpfile)'
	echo ':Tiny-Fu:E::scm::tiny-fu:' >> '$(DESTDIR)$(tinyfuinterpfile)'

install-data-local: install-env-file install-interp-file

uninstall-local:
	rm -f '$(DESTDIR)$(tinyfuenvfile)' '$(DESTDIR)$(tinyfuinterpfile)'


EXTRA_DIST = \
	AUTHORS             \
	ChangeLog           \
	COPYING             \
	MAINTAINERS         \
	NEWS                \
	TODO                \
	autogen.sh          \
	debug-tiny-fu.txt   \
	intltool-extract.in \
	intltool-merge.in   \
	intltool-update.in

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge   \
	intltool-update
