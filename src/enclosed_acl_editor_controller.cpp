/*
   Eiciel - GNOME editor of ACL file permissions.
   Copyright (C) 2019-2022 Roger Ferrer Ibáñez

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

#include "eiciel/enclosed_acl_editor_controller.h"

namespace eiciel {

EnclosedACLEditorController::EnclosedACLEditorController()
    : acl_list_file_controller(this), acl_list_directory_controller(this),
      participant_list_controller(this) {}

void EnclosedACLEditorController::ACLListDirectoryController::
    confirmed_toggle_edit_default_acl(bool new_state) {
  default_acl_are_being_edited(new_state);
  if (!new_state) {
    remove_all_default_entries();
    update_acl_ineffective(get_mask_permissions(),
                           get_default_mask_permissions());
  } else {
    populate_required_default_entries();
  }
}

void EnclosedACLEditorController::ACLListDirectoryController::
    requested_toggle_edit_default_acl(bool requested_new_state) {
  try {
    if (!requested_new_state) {
      Glib::ustring s(
          _("Are you sure you want to remove all ACL default entries?"));
      Gtk::Window *toplevel =
          dynamic_cast<Gtk::Window *>(controller->view->get_root());
      Gtk::MessageDialog *remove_acl_message;
      if (toplevel == NULL) {
        remove_acl_message = new Gtk::MessageDialog(
            s, false, Gtk::MessageType::QUESTION, Gtk::ButtonsType::YES_NO);
      } else {
        remove_acl_message = new Gtk::MessageDialog(*toplevel, s, false,
                                                    Gtk::MessageType::QUESTION,
                                                    Gtk::ButtonsType::YES_NO);
      }
      remove_acl_message->set_modal(true);
      remove_acl_message->signal_response().connect(
          [this, remove_acl_message](int response) mutable {
            if (response == Gtk::ResponseType::YES) {
              confirmed_toggle_edit_default_acl(false);
            }
            delete remove_acl_message;
            remove_acl_message = nullptr;
          });
      remove_acl_message->show();
    } else {
      confirmed_toggle_edit_default_acl(true);
    }
  } catch (ACLManagerException e) {
  }
}

bool EnclosedACLEditorController::ACLListDirectoryController::remove_acl(
    const Glib::ustring &entry_name, ElementKind e) {
  return true;
}

void EnclosedACLEditorController::ACLListDirectoryController::update_acl_entry(
    ElementKind e, const Glib::ustring &name, bool reading, bool writing,
    bool execution) {
  update_acl_ineffective(get_mask_permissions(),
                         get_default_mask_permissions());
}

void EnclosedACLEditorController::ACLListFileController::
    requested_toggle_edit_default_acl(bool) {
  g_warn_if_reached();
}

bool EnclosedACLEditorController::ACLListFileController::remove_acl(
    const Glib::ustring &entry_name, ElementKind e) {
  return true;
}

void EnclosedACLEditorController::ACLListFileController::update_acl_entry(
    ElementKind e, const Glib::ustring &name, bool reading, bool writing,
    bool execution) {
  update_acl_ineffective(get_mask_permissions(),
                         get_default_mask_permissions());
}

void EnclosedACLEditorController::EnclosedParticipantListController::add_acl_entry(
    AddParticipantTarget target, const Glib::ustring &s, ElementKind e,
    bool is_default) {
  ACLListController *acl_controller;
  switch (target) {
  default:
    g_warn_if_reached();
    return;
  case AddParticipantTarget::TO_DIRECTORY:
    acl_controller = controller->get_acl_list_directory_controller();
    break;
  case AddParticipantTarget::TO_FILE:
    acl_controller = controller->get_acl_list_file_controller();
    break;
  }

  if (is_default) {
    if (e == ElementKind::acl_user) {
      e = ElementKind::default_acl_user;
    } else if (e == ElementKind::acl_group) {
      e = ElementKind::default_acl_group;
    }
  }

  switch (e) {
  case ElementKind::acl_user: {
    acl_controller->insert_user(s);
    break;
  }
  case ElementKind::acl_group: {
    acl_controller->insert_group(s);
    break;
  }
  case ElementKind::default_acl_user: {
    acl_controller->insert_default_user(s);
    break;
  }
  case ElementKind::default_acl_group: {
    acl_controller->insert_default_group(s);
    break;
  }
  default:
    break;
  }

  acl_controller->highlight_acl_entry(s, e);
}

void EnclosedACLEditorController::compute_acl_textual_representation() {
  acl_list_directory_controller.get_textual_representation(
      directory_access_acl_text, directory_default_acl_text);
  Glib::ustring dummy; // Files do not have default ACL
  acl_list_file_controller.get_textual_representation(file_access_acl_text,
                                                      dummy);
}

} // namespace eiciel
