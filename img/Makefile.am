eicielimagesdir = $(pkgdatadir)/img

dist_eicielimages_DATA = \
    others-default.png \
	others.png \
	group-acl-default.png \
	group-acl.png \
	group-default.png \
	group.png \
	mask-default.png \
	mask.png \
	user-acl-default.png \
	user-acl.png \
	user-default.png \
	user.png \
	$(END)

EXTRA_DIST = 
EXTRA_DIST += \
	icon_eiciel_16.png \
	icon_eiciel_24.png \
	icon_eiciel_32.png \
	icon_eiciel_48.png \
	icon_eiciel_64.png \
	icon_eiciel_96.png \
	icon_eiciel.svg \
	$(END)

hicolor_iconsdir=$(datadir)/icons/hicolor

install-data-local:
	for icon_size in 16 24 32 48 64 96;\
	do \
		$(INSTALL_DATA) -D $(srcdir)/icon_eiciel_$${icon_size}.png $(DESTDIR)$(hicolor_iconsdir)/$${icon_size}x$${icon_size}/apps/eiciel.png; \
	done
	$(INSTALL_DATA) -D $(srcdir)/icon_eiciel.svg $(DESTDIR)$(hicolor_iconsdir)/scalable/apps/eiciel.svg

uninstall-local:
	for icon_size in 16 24 32 48 64 96;\
	do \
		rm -f $(DESTDIR)$(hicolor_iconsdir)/$${icon_size}x$${icon_size}/apps/eiciel.png; \
	done
	rm -f $(DESTDIR)$(hicolor_iconsdir)/scalable/apps/eiciel.svg
