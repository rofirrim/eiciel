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
#include "config.hpp"
#include <gtkmm.h>
#include <glib/gi18n.h>
#ifdef USING_GNOME2
#include <libgnome/libgnome.h>
#endif
#include "eiciel_container.hpp"

int main(int argc, char* argv[])
{
#ifdef ENABLE_NLS
	setlocale(LC_ALL, "");
	bindtextdomain("eiciel", DATADIR "/locale");
	textdomain("eiciel");

	// We want translations in UTF-8
	bind_textdomain_codeset ("eiciel", "UTF-8");
#endif
#ifdef USING_GNOME2
	GnomeProgram *my_app;
	my_app = gnome_program_init(PACKAGE, VERSION,
            LIBGNOME_MODULE, argc, argv, 
            GNOME_PARAM_HUMAN_READABLE_NAME, "Eiciel",
            GNOME_PARAM_APP_DATADIR, DATADIR,
            NULL);
#else
	bool version_called = false;
	Glib::OptionEntry version_entry;
	version_entry.set_long_name("version");
	version_entry.set_short_name('v');
	version_entry.set_description(_("Print version information"));

	Glib::OptionGroup main_group("Eiciel", _("Access control list editor"));
	main_group.add_entry(version_entry, version_called);
	Glib::OptionGroup gtk_group(gtk_get_option_group(TRUE));

	Glib::OptionContext oc("[FILE]");
	oc.set_main_group(main_group);
	oc.add_group(gtk_group);
	oc.parse(argc, argv);
	if (version_called)
	{
		std::cerr << "GNOME " PACKAGE " " VERSION << std::endl;
		return 0;
	}
#endif

	Gtk::Main kit(argc, argv);

	EicielContainer w;

	if (argc > 1)
	{
		w.open_file(argv[1]);
	}

	kit.run(w);

	return 0;
}

