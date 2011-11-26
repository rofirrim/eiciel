SUBDIRS=figures

eicieldocdir = $(datadir)/gnome/help/eiciel/C


eicieldoc_DATA = eiciel.xml

EXTRA_DIST = eiciel.xml.in

eiciel.xml : eiciel.xml.in
	sed -e "s|@PKG_VERSION@|$(VERSION)|" $(srcdir)/eiciel.xml.in > eiciel.xml

CLEANFILES = 

CLEANFILES += eiciel.xml
