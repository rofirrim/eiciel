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

#include "eiciel/acl_list_controller.h"
#include "eiciel/acl_list_widget.h"

namespace eiciel {

ACLListController ::~ACLListController() {}

void ACLListController::update_acl_ineffective(
    permissions_t effective_permissions,
    permissions_t effective_default_permissions) {
  return view->update_acl_ineffective(effective_permissions,
                                      effective_default_permissions);
}

void ACLListController::create_acl_list_store() {
  return view->create_acl_list_store();
}

void ACLListController::empty_acl_list() { return view->empty_acl_list(); }

void ACLListController::add_non_removable(Glib::ustring title, bool reading,
                                           bool writing, bool execution,
                                           ElementKind e) {
  return view->add_non_removable(title, reading, writing, execution, e);
}

void ACLListController::add_removable(Glib::ustring title, bool reading,
                                       bool writing, bool execution,
                                       ElementKind e) {
  return view->add_removable(title, reading, writing, execution, e);
}

void ACLListController::can_edit_default_acl(bool b) {
  return view->can_edit_default_acl(b);
}

void ACLListController::default_acl_are_being_edited(bool b) {
  return view->default_acl_are_being_edited(b);
}

void ACLListController::replace_acl_store() {
  return view->replace_acl_store();
}

void ACLListController::set_readonly(bool b) { return view->set_readonly(b); }

void ACLListController::set_active(bool b) { return view->set_active(b); }

void ACLListController::insert_user(const Glib::ustring &s) {
  return view->insert_user(s);
}

void ACLListController::insert_group(const Glib::ustring &s) {
  return view->insert_group(s);
}

void ACLListController::insert_default_user(const Glib::ustring &s) {
  return view->insert_default_user(s);
}

void ACLListController::insert_default_group(const Glib::ustring &s) {
  return view->insert_default_group(s);
}

void ACLListController::remove_entry(const Glib::ustring &s, ElementKind e) {
  return view->remove_entry(s, e);
}

void ACLListController::remove_all_default_entries() {
  return view->remove_all_default_entries();
}

void ACLListController::populate_required_default_entries() {
  return view->populate_required_default_entries();
}

void ACLListController::get_textual_representation(Glib::ustring &access_acl,
                                                   Glib::ustring &default_acl) {
  return view->get_textual_representation(access_acl, default_acl);
}

permissions_t ACLListController::get_mask_permissions() {
  return view->get_mask_permissions();
}

permissions_t ACLListController::get_default_mask_permissions() {
  return view->get_default_mask_permissions();
}

void ACLListController::highlight_acl_entry(const Glib::ustring &name,
                                            ElementKind e) {
  return view->highlight_acl_entry(name, e);
}

void ACLListController::fill_acl_list(ACLManager &acl_manager,
                                      bool include_default_entries) {
  permissions_t perms = acl_manager.get_user();
  std::vector<acl_entry> vACL;
  add_non_removable(Glib::locale_to_utf8(acl_manager.get_owner_name()),
                     perms.reading, perms.writing, perms.execution,
                     ElementKind::user);

  vACL = acl_manager.get_acl_user();
  for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
       i++) {
    add_removable(Glib::locale_to_utf8(i->name), i->reading, i->writing,
                   i->execution, ElementKind::acl_user);
  }

  perms = acl_manager.get_group();
  add_non_removable(Glib::locale_to_utf8(acl_manager.get_group_name()),
                     perms.reading, perms.writing, perms.execution,
                     ElementKind::group);

  vACL = acl_manager.get_acl_group();
  for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
       i++) {
    add_removable(Glib::locale_to_utf8(i->name), i->reading, i->writing,
                   i->execution, ElementKind::acl_group);
  }

  if (acl_manager.has_mask()) {
    perms = acl_manager.get_mask();
    add_non_removable(_("Mask"), perms.reading, perms.writing, perms.execution,
                       ElementKind::mask);
  }

  perms = acl_manager.get_other();
  add_non_removable(_("Other"), perms.reading, perms.writing, perms.execution,
                     ElementKind::others);

  can_edit_default_acl(acl_manager.is_directory());
  default_acl_are_being_edited(false);

  if (include_default_entries && acl_manager.is_directory()) {
    bool there_is_default_acl = false;
    permissions_t effective_default_permissions(7);
    if (acl_manager.has_default_mask()) {
      effective_default_permissions = acl_manager.get_mask_default();
    }

    if (acl_manager.has_default_user()) {
      perms = acl_manager.get_user_default();
      add_non_removable(Glib::locale_to_utf8(acl_manager.get_owner_name()),
                         perms.reading, perms.writing, perms.execution,
                         ElementKind::default_user);
      there_is_default_acl = true;
    }

    vACL = acl_manager.get_acl_user_default();

    there_is_default_acl |= (vACL.size() > 0);
    for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
         i++) {
      add_removable(Glib::locale_to_utf8(i->name), i->reading, i->writing,
                     i->execution, ElementKind::default_acl_user);
    }

    if (acl_manager.has_default_group()) {
      perms = acl_manager.get_group_default();
      add_non_removable(Glib::locale_to_utf8(acl_manager.get_group_name()),
                         perms.reading, perms.writing, perms.execution,
                         ElementKind::default_group);
      there_is_default_acl |= true;
    }

    vACL = acl_manager.get_acl_group_default();

    there_is_default_acl |= (vACL.size() > 0);
    for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
         i++) {
      add_removable(Glib::locale_to_utf8(i->name), i->reading, i->writing,
                     i->execution, ElementKind::default_acl_group);
    }

    if (acl_manager.has_default_mask()) {
      perms = acl_manager.get_mask_default();
      add_non_removable(_("Default Mask"), perms.reading, perms.writing,
                         perms.execution, ElementKind::default_mask);
      there_is_default_acl |= true;
    }

    if (acl_manager.has_default_other()) {
      perms = acl_manager.get_other_default();
      add_non_removable(_("Default Other"), perms.reading, perms.writing,
                         perms.execution, ElementKind::default_others);
      there_is_default_acl |= true;
    }
    default_acl_are_being_edited(there_is_default_acl);
  }
}

} // namespace eiciel
