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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
   USA
*/
#ifndef EICIEL_MAIN_WINDOW_CONTROLLER_HPP
#define EICIEL_MAIN_WINDOW_CONTROLLER_HPP

#include "acl_element_kind.hpp"
#include "acl_manager.hpp"
#include "eiciel_acl_list.hpp"
#include "eiciel_participant_target.hpp"
// TODO: Remove
#include "eiciel_acl_list_controller.hpp"
#include "eiciel_main_window.hpp"
#include "eiciel_participant_list_controller.hpp"
#include <config.hpp>
#include <glib/gi18n-lib.h>
#include <set>

class EicielACLWindow;

class EicielACLWindowController : public sigc::trackable, public EicielACLListController, public EicielParticipantListController {
private:
    ACLManager* _ACL_manager;
    EicielACLWindow* _window;
    bool _is_file_opened;
    std::string _current_filename;
    Glib::ustring _last_error_message;

    void update_acl_list();
    void redraw_acl_list();

    void check_editable();

    bool is_directory();

    void set_readonly(bool b);

    void fill_acl_list(Glib::RefPtr<Gtk::ListStore> ref_acl_list, bool include_default_entries);

    void can_edit_enclosed_files(bool b);

public:
    void open_file(const std::string& s);
    Glib::ustring last_error();
    bool opened_file();
    // TODO: Remove
    friend class EicielACLWindow;

    EicielACLWindowController();
    ~EicielACLWindowController();

    void set_active(bool);

    // EicielACLListController
    bool toggle_edit_default_acl(bool default_acl_were_being_edited) override;
    void remove_acl(const std::string& entry_name, ElementKind e) override;
    void update_acl_entry(ElementKind e,
        const std::string& name,
        bool reading,
        bool writing,
        bool execution) override;

    void edit_enclosed_files();

    // EicielParticipantListController
    void add_acl_entry(AddParticipantTarget target, const std::string& s, ElementKind e, bool is_default) override;
};

#endif
