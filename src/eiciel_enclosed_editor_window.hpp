/*
   Eiciel - GNOME editor of ACL file permissions.
   Copyright (C) 2019 Roger Ferrer Ibáñez

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

#ifndef EICIEL_ENCLOSED_EDITOR_WINDOW_HPP
#define EICIEL_ENCLOSED_EDITOR_WINDOW_HPP

#include <config.hpp>
#include <gtkmm.h>
#include <glib/gi18n-lib.h>
#include "eiciel_acl_list.hpp"
#include "eiciel_participant_list.hpp"

class EicielEnclosedEditorController;

class EicielEnclosedEditor : public Gtk::Dialog {
public:
    EicielEnclosedEditor(Gtk::Window& parent, EicielEnclosedEditorController* cont);
    virtual ~EicielEnclosedEditor();

private:
    /* GUI components */
    // dialog headerbar
    Gtk::Button _cancel_changes;
    // headerbar title
    Gtk::Button _apply_changes;
    // dialog vbox
    /* |- */ Gtk::Box _main_box;
    /* |- */ Gtk::Box _acl_editors_hbox;
    /* | |- */ Gtk::Box _left_box;
    /* | | |- */ Gtk::Label _label_acl_directories;
    /* | | |- */ EicielACLList _acl_list_directories;
    /* | |- */ Gtk::Box _right_box;
    /* |   |- */ Gtk::Label _label_acl_files;
    /* |   |- */ EicielACLList _acl_list_files;
    /* |- */ Gtk::Box _participants_box;
    /*   |- */ Gtk::Label _label_participants;
    /*   |- */ EicielParticipantList _participant_list;

    EicielEnclosedEditorController* _controller;

    void apply_changes();
};

#endif // EICIEL_ENCLOSED_EDITOR_WINDOW_HPP
