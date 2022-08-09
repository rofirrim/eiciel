/*
    Eiciel - GNOME editor of Participant file permissions.
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

#include "eiciel/xattr_editor_controller.h"
#include "eiciel/xattr_editor_widget.h"

namespace eiciel {

void XAttrEditorController::open_file(const Glib::ustring &str) {
  try {
    xattr_manager = new XAttrManager(str);
    set_active(true);
    is_opened_file = true;

    fill_attributes();

    check_editable();
  } catch (...) {
    delete xattr_manager;
    xattr_manager = nullptr;
    set_active(false);
    is_opened_file = false;
  }
}

void XAttrEditorController::set_active(bool b) { view->set_active(b); }

void XAttrEditorController::check_editable() {
  // Check that it is the owner or root.
  uid_t real_user = getuid();
  if ((real_user != 0) && (real_user != xattr_manager->get_owner_uid())) {
    view->set_readonly(true);
  } else {
    view->set_readonly(false);
  }
}

void XAttrEditorController::fill_attributes() {
  g_return_if_fail(view);
  g_return_if_fail(xattr_manager);

  view->clear_attributes();

  auto attributes = xattr_manager->get_attributes_list();
  for (const auto &item : attributes) {
    view->add_attribute(item.first, item.second);
  }
}

bool XAttrEditorController::add_attribute(const Glib::ustring &name,
                                          const Glib::ustring &value) {
  g_return_val_if_fail(xattr_manager, false);

  try {
    xattr_manager->add_attribute(name, value);
  } catch (...) {
    return false;
  }

  return true;
}

bool XAttrEditorController::remove_attribute(const Glib::ustring &name) {
  g_return_val_if_fail(xattr_manager, false);

  try {
    xattr_manager->remove_attribute(name);
  } catch (...) {
    return false;
  }

  return true;
}

bool XAttrEditorController::update_attribute_value(const Glib::ustring &name,
                                                   const Glib::ustring &value) {

  return add_attribute(name, value);
}

} // namespace eiciel