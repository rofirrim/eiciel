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

#ifndef XATTR_EDITOR_CONTROLLER_H
#define XATTR_EDITOR_CONTROLLER_H

#include "eiciel/xattr_manager.h"

namespace eiciel {

class XAttrEditorWidget;

class XAttrEditorController {
public:
  void set_view(XAttrEditorWidget *v) { view = v; }
  void open_file(const Glib::ustring &str);
  
  bool opened_file() const { return is_opened_file; }
  
  void set_active(bool b);

  bool add_attribute(const Glib::ustring &name, const Glib::ustring &attr);
  bool update_attribute_value(const Glib::ustring &name, const Glib::ustring &attr);
  bool remove_attribute(const Glib::ustring &name);

private:
  void check_editable();
  void fill_attributes();

  XAttrEditorWidget *view = nullptr;
  XAttrManager *xattr_manager = nullptr;
  bool is_opened_file = false;
};
} // namespace eiciel

#endif // XATTR_EDITOR_CONTROLLER_H