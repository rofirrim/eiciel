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

#include "eiciel_enclosed_editor_window_controller.hpp"

EicielEnclosedEditorController::EicielEnclosedEditorController()
    : _acl_list_directory_controller(this)
    , _acl_list_file_controller(this)
    , _participant_list_controller(this)
{
}

bool EicielEnclosedEditorController::ACLListDirectoryController::toggle_edit_default_acl(bool default_acl_were_being_edited)
{
    if (default_acl_were_being_edited) {
        Glib::ustring s(
            _("Are you sure you want to remove all ACL default entries?"));
        Gtk::Container* toplevel = _editor_controller->_window->get_toplevel();
        int result;
        if (toplevel == NULL || !toplevel->get_is_toplevel()) {
            Gtk::MessageDialog remove_acl_message(s, false, Gtk::MESSAGE_QUESTION,
                Gtk::BUTTONS_YES_NO);
            result = remove_acl_message.run();
        } else {
            Gtk::MessageDialog remove_acl_message(*(Gtk::Window*)toplevel, s, false,
                Gtk::MESSAGE_QUESTION,
                Gtk::BUTTONS_YES_NO);
            result = remove_acl_message.run();
        }
        if (result == Gtk::RESPONSE_NO)
            return false;
        remove_all_default_entries();
    } else {
        populate_required_default_entries();
    }
    return true;
}

void EicielEnclosedEditorController::ACLListDirectoryController::remove_acl(const std::string& entry_name, ElementKind e)
{
    remove_entry(entry_name, e);
}

void EicielEnclosedEditorController::ACLListDirectoryController::update_acl_entry(ElementKind e,
    const std::string& name,
    bool reading,
    bool writing,
    bool execution)
{
    update_acl_ineffective(get_mask_permissions(), get_default_mask_permissions());
}

bool EicielEnclosedEditorController::ACLListFileController::toggle_edit_default_acl(bool)
{
    // This should never run but just in case
    return false;
}

void EicielEnclosedEditorController::ACLListFileController::remove_acl(const std::string& entry_name, ElementKind e)
{
    remove_entry(entry_name, e);
}

void EicielEnclosedEditorController::ACLListFileController::update_acl_entry(ElementKind e,
    const std::string& name,
    bool reading,
    bool writing,
    bool execution)
{
    update_acl_ineffective(get_mask_permissions(), get_default_mask_permissions());
}

void EicielEnclosedEditorController::ParticipantListController::add_acl_entry(AddParticipantTarget target, const std::string& s, ElementKind e, bool is_default)
{
    EicielACLListController* acl_controller;
    switch (target) {
    default:
        // FIXME: Emit a warnign
        return;
    case AddParticipantTarget::ADD_PARTICIPANT_TO_DIRECTORY:
        acl_controller = _editor_controller->get_acl_list_directory_controller();
        break;
    case AddParticipantTarget::ADD_PARTICIPANT_TO_FILE:
        acl_controller = _editor_controller->get_acl_list_file_controller();
        break;
    }

    if (is_default) {
        if (e == EK_ACL_USER) {
            e = EK_DEFAULT_ACL_USER;
        } else if (e == EK_ACL_GROUP) {
            e = EK_DEFAULT_ACL_GROUP;
        }
    }

    switch (e) {
    case EK_ACL_USER: {
        acl_controller->insert_user(s);
        break;
    }
    case EK_ACL_GROUP: {
        acl_controller->insert_group(s);
        break;
    }
    case EK_DEFAULT_ACL_USER: {
        acl_controller->insert_default_user(s);
        break;
    }
    case EK_DEFAULT_ACL_GROUP: {
        acl_controller->insert_default_group(s);
        break;
    }
    default:
        break;
    }

    acl_controller->choose_acl(s, e);
}

void EicielEnclosedEditorController::apply_changes()
{
    _acl_list_directory_controller.get_textual_representation(_directory_access_acl_text, _directory_default_acl_text);
    std::string dummy; // Files do not have default ACL
    _acl_list_file_controller.get_textual_representation(_file_access_acl_text, dummy);
    _window->response(Gtk::RESPONSE_APPLY);
}
