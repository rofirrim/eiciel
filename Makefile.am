SUBDIRS = src img po man doc

EXTRA_DIST = config.rpath m4

dist-hook :
	rm -rf `find $(distdir) -name .svn`

ACLOCAL_AMFLAGS = -I m4
