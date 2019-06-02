/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2004-2019 Roger Ferrer Ibáñez

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
#ifndef EICIEL_MAIN_WINDOW_HPP
#define EICIEL_MAIN_WINDOW_HPP

#include "acl_element_kind.hpp"
#include "acl_list.hpp"
#include "eiciel_acl_list.hpp"
#include "eiciel_main_window_controller.hpp"
#include "eiciel_participant_list.hpp"
#include "participant_list.hpp"
#include <config.hpp>
#include <glib/gi18n-lib.h>
#include <gtkmm.h>
#include <set>
#include <string>

class EicielACLWindowController;

class EicielACLWindow : public Gtk::Box {
public:
    EicielACLWindow(EicielACLWindowController* cont);
    virtual ~EicielACLWindow();

    void set_filename(const std::string& filename);

    void initialize(const std::string& s);
    bool opened_file();

    void set_active(bool b);

    Glib::ustring last_error();

    void show_info_bar();
    void hide_info_bar();

    void can_edit_enclosed_files(bool b);
    void set_recursive_progress(double);

private:
    /* GUI components */
    /* |- */ Gtk::InfoBar _info_bar;
    /*  (content area) */
    /*   |- */ Gtk::Box _info_bar_vbox;
    /*     |- */ Gtk::Label _info_bar_label;
    /*     |- */ Gtk::ProgressBar _info_bar_progress;
    /* |- */ Gtk::Box _main_box;
    /*   |- */ Gtk::Label _label_current_acl;
    /*   |- */ EicielACLList _acl_list;
    /*   |- */ Gtk::Label _label_participants;
    /*   |- */ EicielParticipantList _participant_list;
    /*   |- */ Gtk::Button _edit_enclosed_files;

    EicielACLWindowController* _controller;

    void there_is_no_file();

    void edit_enclosed_files();
};

#endif
