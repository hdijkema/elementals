## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src test pod

elementalsdocdir = ${prefix}/doc/elementals
elementalsdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS\
	pod/core.css \
	$(POD_HTML_FILES)


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = $(elementalsdoc_DATA) \
	$(INTLTOOL_FILES)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(elementalsdocdir)


POD_FILES = pod/hash.pod
POD_HTML_FILES = pod/hash.html

CLEANFILES = $(POD_HTML_FILES)

$(POD_HTML_FILES) : $(POD_FILES)
	for p in $(POD_FILES); do h=`echo $$p | sed -e 's/.pod/.html/'`;SPOD=`which spod2html`;if [ "$SPOD" = "" ]; then pod2html --css core.css --infile $$p --outfile $$h; else spod2html --stylesheet=core.css $$p $$h; fi ;done

