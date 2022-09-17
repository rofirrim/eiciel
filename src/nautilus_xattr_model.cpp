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

#include "eiciel/nautilus_xattr_model.h"
#include "eiciel/xattr_manager.h"
#include <gio/gio.h>

static void add_to_list(GListStore *store, const char *name,
                        const char *value) {
  g_list_store_append(store, nautilus_properties_item_new(name, value));
}

NautilusPropertiesModel *eiciel_xattr_model_new(const char *file_name_path) {
  try {

    eiciel::XAttrManager xattr_manager(file_name_path);
    auto attr_list = xattr_manager.get_attributes_list();

    if (attr_list.empty())
      return nullptr;

    GListStore *xattr_model =
        (GListStore *)g_list_store_new(NAUTILUS_TYPE_PROPERTIES_ITEM);

    for (const auto& it : attr_list) {
        add_to_list(xattr_model, it.first.c_str(), it.second.c_str());
    }

    return nautilus_properties_model_new(_("Extended attributes"),
                                         (GListModel *)xattr_model);

  } catch (...) {
    // Catch all to avoid crashing nautilus.
  }

  return nullptr;
}
