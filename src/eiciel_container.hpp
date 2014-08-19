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
#include <config.hpp>
#include <gtkmm.h>
#ifdef USING_GNOME2
#include <libgnome/libgnome.h>
#endif
#include <glib/gi18n-lib.h>

#include "eiciel_main_controller.hpp"
#include "eiciel_main_window.hpp"

#ifdef ENABLE_USER_XATTR
#include "eiciel_xattr_controller.hpp"
#include "eiciel_xattr_window.hpp"
#endif

class EicielContainer : public Gtk::Window
{
    private:
        Gtk::Box _main_container;
        Gtk::Box _ACL_tabpage;
        Gtk::Box _XAttr_tabpage;
        Gtk::Box _top;
        Gtk::Box _bottom;
        Gtk::Button _open_file;
        Gtk::Label _file_label;
        Gtk::Label _file_name;
        Gtk::Button _exit_button;
        Gtk::Button _help;
        Gtk::Button _about;

        Gtk::Notebook _notebook;

        EicielWindow* _main_widget;
        EicielMainController* _widget_controller;

#ifdef ENABLE_USER_XATTR
        EicielXAttrWindow* _xattr_widget;
        EicielXAttrController* _xattr_controller;
#endif
        void quit_application();
        void open_file_();
        Glib::ustring error_message();
        void show_about();
        void show_help();

        // FileChooser stuff
        void chooser_file_activated(Gtk::FileChooserDialog* dialog);

    public:
        EicielContainer();
        bool open_file(Glib::ustring nom);
};

class EicielAboutBox : public Gtk::Dialog
{
    private:
        Gtk::Label _title;
        Gtk::Label _author;
    public:
        EicielAboutBox(Gtk::Window& parent);
};
