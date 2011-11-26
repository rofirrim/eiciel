bin_PROGRAMS = eiciel
eiciel_CPPFLAGS = -I@top_srcdir@/lib -DDATADIR="\"$(datadir)\"" -DPKGDATADIR="\"$(pkgdatadir)\"" 
eiciel_CXXFLAGS = @GTKMM_CFLAGS@ -Wall 
eiciel_LDFLAGS = @GTKMM_LIBS@ @ACL_LIBS@
eiciel_SOURCES = \
    acl_manager.cpp \
	acl_manager.hpp \
	eiciel_main_controller.cpp \
	eiciel_main_controller.hpp \
	eiciel_main_window.cpp \
	eiciel_main_window.hpp \
	acl_list.hpp \
	participant_list.hpp \
	acl_element_kind.hpp \
	cellrenderer_acl.hpp \
	cellrenderer_acl.cpp \
	eiciel_standalone.cpp \
	eiciel_container.hpp \
	eiciel_container.cpp

if ENABLE_USER_XATTR
eiciel_SOURCES += \
	xattr_manager.hpp \
	xattr_manager.cpp \
	eiciel_xattr_window.cpp \
	eiciel_xattr_window.hpp \
	eiciel_xattr_controller.cpp \
	eiciel_xattr_controller.hpp \
	xattr_list_model.hpp
endif

nautilusextdir=@NAUTILUS_EXTENSIONS_DIR@
nautilusext_LTLIBRARIES=libeiciel-nautilus.la
libeiciel_nautilus_la_CPPFLAGS = -I@top_srcdir@/lib -DDATADIR="\"$(datadir)\"" -DPKGDATADIR="\"$(pkgdatadir)\"" 
libeiciel_nautilus_la_CXXFLAGS = @GTKMM_CFLAGS@ -Wall 
libeiciel_nautilus_la_LDFLAGS =  -module -avoid-version @GTKMM_LIBS@ @ACL_LIBS@
libeiciel_nautilus_la_SOURCES = \
    acl_manager.cpp \
	acl_manager.hpp \
	eiciel_nautilus_page.cpp \
	eiciel_nautilus_page.hpp \
	eiciel_main_controller.cpp \
	eiciel_main_controller.hpp \
	eiciel_main_window.cpp \
	eiciel_main_window.hpp \
	xattr_list_model.hpp \
	participant_list.hpp \
	acl_element_kind.hpp \
	cellrenderer_acl.hpp \
	cellrenderer_acl.cpp 

if ENABLE_USER_XATTR
libeiciel_nautilus_la_SOURCES += \
	xattr_manager.hpp \
	xattr_manager.cpp \
	eiciel_xattr_window.cpp \
	eiciel_xattr_window.hpp \
	eiciel_xattr_controller.cpp \
	eiciel_xattr_controller.hpp \
	xattr_list_model.hpp
endif

desktopfilesdir=@datadir@/applications

desktopfiles_DATA=eiciel.desktop

eiciel.desktop : eiciel.desktop.in
	sed -e "s|@DSK_VERSION@|$(VERSION)|;s|@DSK_bindir@|$(bindir)|;s|@DSK_pkgdatadir@|$(pkgdatadir)|" $< > $@

CLEANFILES = 

# Since 'make' built it, 'make clean' should clean it
# see http://sources.redhat.com/automake/automake.html#Clean
CLEANFILES += eiciel.desktop

EXTRA_DIST= \
	eiciel.desktop.in
