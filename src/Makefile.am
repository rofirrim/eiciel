common_sources = \
				 acl_manager.cpp \
				 acl_manager.hpp \
				 eiciel_main_window.cpp \
				 eiciel_main_window.hpp \
				 eiciel_main_window_controller.cpp \
				 eiciel_main_window_controller.hpp \
				 eiciel_acl_list.cpp \
				 eiciel_acl_list.hpp \
				 eiciel_acl_list_controller.cpp \
				 eiciel_acl_list_controller.hpp \
				 eiciel_participant_list.cpp \
				 eiciel_participant_list.hpp \
				 eiciel_participant_list_controller.cpp \
				 eiciel_participant_list_controller.hpp \
				 eiciel_participant_target.hpp \
				 eiciel_enclosed_editor_window_controller.hpp \
				 eiciel_enclosed_editor_window_controller.cpp \
				 eiciel_enclosed_editor_window.hpp \
				 eiciel_enclosed_editor_window.cpp \
				 acl_list.hpp \
				 participant_list.hpp \
				 acl_element_kind.hpp \
				 cellrenderer_acl.hpp \
				 cellrenderer_acl.cpp

if ENABLE_USER_XATTR
common_sources += \
	xattr_manager.hpp \
	xattr_manager.cpp \
	eiciel_xattr_window.cpp \
	eiciel_xattr_window.hpp \
	eiciel_xattr_controller.cpp \
	eiciel_xattr_controller.hpp \
	xattr_list_model.hpp
endif

bin_PROGRAMS = eiciel
eiciel_CPPFLAGS = -I@top_srcdir@/lib -DDATADIR="\"$(datadir)\"" -DPKGDATADIR="\"$(pkgdatadir)\"" 
eiciel_CXXFLAGS = @GTKMM_CFLAGS@ -Wall 
eiciel_LDFLAGS = @GTKMM_LIBS@ @ACL_LIBS@
eiciel_SOURCES = \
				 $(common_sources) \
				 eiciel_standalone.cpp \
				 eiciel_container.hpp \
				 eiciel_container.cpp


if ENABLE_NAUTILUS_EXTENSION
nautilusextdir=@NAUTILUS_EXTENSIONS_DIR@
nautilusext_LTLIBRARIES=libeiciel-nautilus.la
libeiciel_nautilus_la_CPPFLAGS = -I@top_srcdir@/lib -DDATADIR="\"$(datadir)\"" -DPKGDATADIR="\"$(pkgdatadir)\"" 
libeiciel_nautilus_la_CXXFLAGS = @GTKMM_CFLAGS@ @NAUTILUS_EXTENSION_CFLAGS@ -Wall
libeiciel_nautilus_la_LDFLAGS =  -module -avoid-version @GTKMM_LIBS@ @NAUTILUS_EXTENSION_LIBS@ @ACL_LIBS@
libeiciel_nautilus_la_SOURCES = \
								$(common_sources) \
								eiciel_nautilus_page.cpp \
								eiciel_nautilus_page.hpp
endif

desktopfilesdir=@datadir@/applications
desktopfiles_DATA=org.roger-ferrer.Eiciel.desktop


appdatadir = $(datarootdir)/metainfo
dist_appdata_DATA = org.roger-ferrer.Eiciel.appdata.xml

org.roger-ferrer.Eiciel.desktop : eiciel.desktop.in
	$(AM_V_GEN)sed -e "s|@DSK_VERSION@|1.1|;s|@DSK_bindir@|$(bindir)|;s|@DSK_pkgdatadir@|$(pkgdatadir)|" $< > $@

CLEANFILES = 

# Since 'make' built it, 'make clean' should clean it
# see http://sources.redhat.com/automake/automake.html#Clean
CLEANFILES += eiciel.desktop

EXTRA_DIST= \
	eiciel.desktop.in \
	org.roger-ferrer.Eiciel.appdata.xml
