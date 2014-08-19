/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2004-2014 Roger Ferrer Ibáñez

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
#include "eiciel_container.hpp"

EicielContainer::EicielContainer()
    : _main_container(Gtk::ORIENTATION_VERTICAL),
    _ACL_tabpage(Gtk::ORIENTATION_VERTICAL),
    _XAttr_tabpage(Gtk::ORIENTATION_VERTICAL),
    _open_file(Gtk::Stock::OPEN), 
    _file_label(_("<b>File name</b>")),
    _file_name(_("No file opened")),
    _exit_button(Gtk::Stock::QUIT),
    _help(Gtk::Stock::HELP),
    _about(_("About..."))
{

    set_title("Eiciel");
    set_border_width(4);

    set_default_icon_name("eiciel");

    add(_main_container);

    _main_container.pack_start(_top, Gtk::PACK_SHRINK, 2);
    _file_label.set_use_markup();
    _top.pack_start(_file_label, Gtk::PACK_SHRINK, 2);
    _top.pack_start(_file_name, Gtk::PACK_EXPAND_WIDGET, 4);
    _top.pack_start(_open_file, Gtk::PACK_SHRINK, 4);
    _top.set_homogeneous(false);

    _main_container.pack_start(_notebook);
    _notebook.append_page(_ACL_tabpage, _("Access Control List"));

    _widget_controller = new EicielMainController();
    _main_widget = new EicielWindow(_widget_controller);

    _ACL_tabpage.pack_start(*_main_widget, Gtk::PACK_EXPAND_WIDGET, 0);

#ifdef ENABLE_USER_XATTR
    _notebook.append_page(_XAttr_tabpage, _("Extended user attributes"));

    _xattr_controller = new EicielXAttrController();
    _xattr_widget = new EicielXAttrWindow(_xattr_controller);

    _XAttr_tabpage.pack_start(*_xattr_widget, Gtk::PACK_EXPAND_WIDGET, 0);
#endif  

    _main_container.pack_start(_bottom, Gtk::PACK_SHRINK, 2);
    _bottom.set_spacing(4);
    _bottom.pack_end(_about);
    _bottom.pack_end(_help);
    _bottom.pack_end(_exit_button);

    _exit_button.signal_clicked().connect(
            sigc::mem_fun(*this, &EicielContainer::quit_application)
            );

    _about.signal_clicked().connect (
            sigc::mem_fun(*this, &EicielContainer::show_about)
            );

    _help.signal_clicked().connect (
             sigc::mem_fun(*this, &EicielContainer::show_help)
            );

    _open_file.signal_clicked().connect(
            sigc::mem_fun(*this, &EicielContainer::open_file_)
            );

    show_all_children();
}

void EicielContainer::show_help()
{
    GError* error = NULL;

#ifdef USING_GNOME2
    gnome_help_display("eiciel", NULL, &error);
#else
    gtk_show_uri(this->get_screen()->gobj(), "ghelp:eiciel", GDK_CURRENT_TIME, &error);
#endif
    if (error != NULL)
    {
        g_warning(_("Could not show the help file: %s"), error->message);
        g_error_free(error);
    }
}

void EicielContainer::quit_application()
{
    Gtk::Main::quit();
}

void EicielContainer::show_about()
{
    EicielAboutBox about(*this);
    about.run();
}

void EicielContainer::chooser_file_activated(Gtk::FileChooserDialog* dialog)
{
    dialog->response(Gtk::RESPONSE_NONE);
}

void EicielContainer::open_file_()
{
    Gtk::FileChooserDialog dialog(_("Choose a file or a directory"), 
            Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_local_only(true);
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_NONE);
    dialog.set_transient_for(*this);

    dialog.signal_file_activated().connect(
            sigc::bind<Gtk::FileChooserDialog*>(
                sigc::mem_fun(*this, &EicielContainer::chooser_file_activated), &dialog)
            );
    
    int result = dialog.run();
    dialog.hide();
    switch (result)
    {
        case(Gtk::RESPONSE_NONE):
            {
                if (!this->open_file(dialog.get_filename()))
                {
                    Gtk::MessageDialog message(
                            *this,
                            _("Could not open the file \"") 
                            + Glib::locale_to_utf8(dialog.get_filename()) + Glib::ustring("\" ") 
                            + Glib::ustring("(") + _widget_controller->last_error() + Glib::ustring(")"),
                            false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
                    message.run();
                }
                break;
            }
        case(Gtk::RESPONSE_CANCEL):
            {
                break;
            }
        default:
            {
                break;
            }
    }
}

bool EicielContainer::open_file(Glib::ustring nom)
{
    bool result;
    _widget_controller->open_file(nom);

    result = _widget_controller->opened_file();

#ifdef ENABLE_USER_XATTR
    _xattr_controller->open_file(nom);

    result |= _xattr_controller->opened_file();
#endif

    if (result)
    {
        _file_name.set_text(nom);
    }
    else
    {
        _file_name.set_text(_("No file opened"));
    }

    return result;
}

/* About box */
EicielAboutBox::EicielAboutBox(Gtk::Window& parent)
#ifdef USING_GNOME2
    : Gtk::Dialog (_("About..."), parent, true, true),
#else
    : Gtk::Dialog (_("About..."), parent, true),
#endif
    _title("<span size=\"xx-large\"><b>Eiciel " PACKAGE_VERSION "</b></span>"),
    _author("<small>Copyright Â© 2004-2005 Roger Ferrer IbÃ¡Ã±ez</small>")
{
    set_border_width(4);
    get_vbox()->set_spacing(4);
    _title.set_use_markup();
    _author.set_use_markup();
    get_vbox()->add(_title);
    get_vbox()->add(_author);
    add_button(Gtk::Stock::OK, 0);
    show_all_children();
}
