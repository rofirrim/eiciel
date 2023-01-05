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

#ifndef EICIEL_ACL_LIST_CONTROLLER_H
#define EICIEL_ACL_LIST_CONTROLLER_H

#include "config.h"

#include "eiciel/i18n.h"

#include "eiciel/acl_element_kind.h"
#include "eiciel/acl_manager.h"
#include <gtkmm.h>
#include <string>

namespace eiciel {

class ACLListWidget;

class ACLListController {
public:
  virtual ~ACLListController();

  void update_acl_ineffective(permissions_t effective_permissions,
                              permissions_t effective_default_permissions);
  void create_acl_list_store();
  void empty_acl_list();
  void add_non_removable(Glib::ustring title, bool reading, bool writing,
                          bool execution, ElementKind e);
  void add_removable(Glib::ustring title, bool reading, bool writing,
                      bool execution, ElementKind e);
  void can_edit_default_acl(bool b);
  void default_acl_are_being_edited(bool b);
  void replace_acl_store();
  void set_readonly(bool b);
  void set_active(bool b);

  void insert_user(const Glib::ustring &s);
  void insert_group(const Glib::ustring &s);
  void insert_default_user(const Glib::ustring &s);
  void insert_default_group(const Glib::ustring &s);
  void populate_required_default_entries();
  void remove_all_default_entries();

  void get_textual_representation(Glib::ustring &access_acl,
                                  Glib::ustring &default_acl);
  void highlight_acl_entry(const Glib::ustring& name, ElementKind e);

  permissions_t get_mask_permissions();
  permissions_t get_default_mask_permissions();

  void fill_acl_list(ACLManager&, bool include_default_entries);

  // Events
  virtual void requested_toggle_edit_default_acl(bool requested_new_state) = 0;
  virtual bool remove_acl(const Glib::ustring &entry_name, ElementKind e) = 0;
  virtual void update_acl_entry(ElementKind e, const Glib::ustring &name,
                                bool reading, bool writing, bool execution) = 0;

  void set_view(ACLListWidget *v) { view = v; }

private:
  ACLListWidget *view;

  void remove_entry(const Glib::ustring &s, ElementKind e);
};

} // namespace eiciel

#endif // EICIEL_ACL_LIST_CONTROLLER_H
