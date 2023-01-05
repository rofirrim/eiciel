/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2022 Roger Ferrer Ibáñez

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

#ifndef EICIEL_ACL_LIST_WIDGET_H
#define EICIEL_ACL_LIST_WIDGET_H

#include "config.h"

#include "eiciel/i18n.h"

#include <unordered_map>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/image.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/columnview.h>
#include "eiciel/acl_item.h"
#include "eiciel/acl_manager.h"
#include "eiciel/acl_element_kind.h"
#include "eiciel/acl_list_controller.h"

namespace eiciel {

enum class ACLListWidgetMode {
  DEFAULT = 0,
  ONLY_FILE,
  ONLY_DIRECTORY,
};

class ACLListWidget : public Gtk::Box {
public:
  ACLListWidget(ACLListController *, ACLListWidgetMode);
  virtual ~ACLListWidget();

  void remove_all_default_entries();
  void populate_required_default_entries();
  void get_textual_representation(Glib::ustring &access_acl,
                                  Glib::ustring &default_acl);
  permissions_t get_mask_permissions();
  permissions_t get_default_mask_permissions();
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
  void insert_user(const Glib::ustring &str);
  void insert_group(const Glib::ustring &str);
  void insert_default_user(const Glib::ustring &str);
  void insert_default_group(const Glib::ustring &str);
  void remove_entry(const Glib::ustring &s, ElementKind e);
  void highlight_acl_entry(const Glib::ustring &name, ElementKind kind);

  static GType get_type() {
    // Let's cache once the type does exist.
    if (!gtype)
      gtype = g_type_from_name("gtkmm__CustomObject_ACLItem");
    return gtype;
  }

private:
  void add_element(Glib::ustring title, bool reading, bool writing,
                   bool execution, ElementKind e);
  void insert_before(const Glib::ustring &s, ElementKind new_element_kind,
                     ElementKind before_element_kind, bool removable);
  void populate_required_nondefault_entries();
  bool nondefault_acl_is_empty();
  void remove_unneeded_entries();
  void change_permissions(Glib::RefPtr<ACLItem> item,
                          Gtk::CheckButton *checkbutton,
                          const Glib::ustring &permission);
  void toggle_edit_default_acl();
  void disable_default_acl_editing();

  void remove_button_signal(Gtk::Button *btn);
  void keep_button_signal(Gtk::Button *btn, sigc::connection c);

  void remove_checkbutton_signal(Gtk::CheckButton *btn);
  void keep_checkbutton_signal(Gtk::CheckButton *btn, sigc::connection c);

  Glib::RefPtr<ACLListItemModel> model;
  Gtk::ColumnView* column_view;

  Gtk::ToggleButton* edit_default_participants;

  Gtk::Image* warning_icon;
  Gtk::Label* warning_label;

  ACLListController *controller;

  Glib::Property<bool> readonly_mode;
  Glib::Property<bool> exist_ineffective_permissions;

  bool toggling_default_acl;

  std::unordered_map<Gtk::Button *, sigc::connection> button_signal_map;
  std::unordered_map<Gtk::CheckButton *, sigc::connection> checkbutton_signal_map;

  ACLListWidgetMode widget_mode;

  static GType gtype;
};

} // namespace eiciel

#endif // EICIEL_ACL_LIST_WIDGET_H
