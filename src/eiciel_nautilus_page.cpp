/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2004-2010 Roger Ferrer Ibáñez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,  USA
*/

#include <config.hpp>
#include <gtkmm.h>
#include <gtk/gtk.h>
#include <libnautilus-extension/nautilus-property-page.h>
#include <libnautilus-extension/nautilus-property-page-provider.h>
#include <gio/gio.h>
#include <glib/gi18n-lib.h>

#include "eiciel_main_controller.hpp"
#include "eiciel_main_window.hpp"
#include "eiciel_nautilus_page.hpp"

#ifdef ENABLE_USER_XATTR
  #include "eiciel_xattr_controller.hpp"
  #include "eiciel_xattr_window.hpp"
#endif

struct NautilusEicielPropertiesPageDetails {
};

typedef struct {
        GObject parent;
} NautilusEicielPropertiesPageProvider;

typedef struct {
        GObjectClass parent;
} NautilusEicielPropertiesPageProviderClass;


static GObjectClass* parent_class = NULL;

static void nautilus_eiciel_property_page_provider_iface_init (NautilusPropertyPageProviderIface* iface);

static GList* nautilus_get_property_pages (NautilusPropertyPageProvider *provider,
                    GList *files);


static void nautilus_eiciel_property_page_provider_iface_init (NautilusPropertyPageProviderIface* iface)
{
	iface->get_pages = nautilus_get_property_pages;
}

static void nautilus_eiciel_properties_page_finalize (GObject* object)
{
}

void nautilus_eiciel_properties_page_class_init(NautilusEicielPropertiesPageClass* neppc)
{
	parent_class = (GObjectClass*)g_type_class_peek_parent (neppc);
	G_OBJECT_CLASS (neppc)->finalize = nautilus_eiciel_properties_page_finalize;
}


// Returns a GList wit the pages we're going to provide
// to Nautilus 
static GList* nautilus_get_property_pages (NautilusPropertyPageProvider *provider,
                    GList *files)
{
	GList *pages = NULL;
	char* uri;
	char* local_file;
	NautilusPropertyPage *real_page;
	NautilusFileInfo* file;

    // Check that only one file has been selected
	if (!files || files->next != NULL) 
	{
		return NULL;
	}
	file = (NautilusFileInfo*)files->data;

	// Get the uri from nautilus
	uri = nautilus_file_info_get_uri (file);

    if (uri == NULL)
    {
        return NULL;
    }

    GFile* guri = g_file_new_for_uri(uri);

	// and check that it is local
	if (!g_file_has_uri_scheme(guri, "file"))
	{
		g_object_unref(guri);
		return NULL;
	}

	// if local, get the path of the file
	local_file = g_file_get_path(guri);
	g_object_unref(guri);

    // Well, some files are local but do not have a real file behind them
	if (local_file == NULL)
	{
		return NULL;
	}

	// Initialize gtkmm
	// TODO: Is this correct ? (it works, though)
	Gtk::Main::init_gtkmm_internals();

	// Now create the controller and the view
	EicielMainController* main_controller = new EicielMainController();
	EicielWindow* eiciel_window = Gtk::manage(new EicielWindow(main_controller));
	// and open the file
	main_controller->open_file(std::string(local_file));

	if (!main_controller->opened_file())
	{
		delete eiciel_window;
	}
	else
	{
		real_page = nautilus_property_page_new
			("EicielPropertyPage::property_page", 
			 gtk_label_new (_("Access Control List")),
			 GTK_WIDGET(eiciel_window->gobj()));
		pages = g_list_append (pages, real_page);
	}

    // Likewise for user_xattr
#ifdef ENABLE_USER_XATTR
	EicielXAttrController* xattr_controller = new EicielXAttrController();
	EicielXAttrWindow* xattr_window = Gtk::manage(new EicielXAttrWindow(xattr_controller));

	xattr_controller->open_file(std::string(local_file));

	if (!xattr_controller->opened_file())
	{
		delete xattr_window;
	}
	else
	{
		real_page = nautilus_property_page_new
			("EicielPropertyPage::property_page", 
			 gtk_label_new (_("Extended user attributes")),
			 GTK_WIDGET(xattr_window->gobj()));
		pages = g_list_append (pages, real_page);
	}
#endif
	

	g_free(local_file);

	return pages;
}

static GType nautilus_eiciel_type = 0;

static void nautilus_eiciel_register_type (GTypeModule *module)
{
	static const GTypeInfo info = {
                sizeof (NautilusEicielPropertiesPageClass),
                (GBaseInitFunc) NULL,
                (GBaseFinalizeFunc) NULL,
				(GClassInitFunc) NULL,
                // (GClassInitFunc) nautilus_eiciel_properties_page_class_init,
                NULL,
                NULL,
                sizeof (NautilusEicielPropertiesPage),
                0,
                (GInstanceInitFunc) NULL,
        };

	static const GInterfaceInfo property_page_provider_iface_info = {
		(GInterfaceInitFunc) nautilus_eiciel_property_page_provider_iface_init,
		NULL,
		NULL
	};
	
	nautilus_eiciel_type = g_type_module_register_type (module, 
						G_TYPE_OBJECT,
						"EicielNautilus",
						&info, (GTypeFlags)0);
	g_type_module_add_interface (module,
				     nautilus_eiciel_type,
				     NAUTILUS_TYPE_PROPERTY_PAGE_PROVIDER,
				     &property_page_provider_iface_info);
}

/*
 * Routines called by the Nautilus loader
 */
extern void nautilus_module_initialize (GTypeModule  *module)
{
	// g_print ("Initializing Eiciel property page\n");
	nautilus_eiciel_register_type(module);
	// g_print ("Initialization done\n");
#ifdef ENABLE_NLS
	setlocale(LC_ALL, "");
    bindtextdomain("eiciel", DATADIR "/locale");
    
    // We want translations in UTF-8, regardless
    // of the actual encoding used for translation
    bind_textdomain_codeset ("eiciel", "UTF-8");
#endif
}

/* Perform module-specific shutdown. */
extern void nautilus_module_shutdown   (void)
{
	// g_print ("Ending Eiciel property page\n");
}

/* List all the extension types.  */
extern void nautilus_module_list_types (const GType **types,
			    int          *num_types)
{
	static GType type_list[1];
	
	type_list[0] = nautilus_eiciel_type;

	*types = type_list;
	*num_types = 1;
}

