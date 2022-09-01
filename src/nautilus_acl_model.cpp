/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 20222 Roger Ferrer Ibáñez

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

#include "eiciel/nautilus_acl_model.h"
#include "eiciel/acl_manager.h"
#include <gio/gio.h>

static const char *permissions_to_name(bool directory,
                                       const eiciel::permissions_t &p,
                                       const eiciel::permissions_t mask = 07) {
  if (!directory) {
    switch (p.to_int() & mask.to_int()) {
    case 07:
      return _("Read, write and execute");
    case 06:
      return _("Read and write");
    case 05:
      return _("Read and execute");
    case 04:
      return _("Only read");
    case 03:
      return _("Write and execute");
    case 02:
      return _("Only write");
    case 01:
      return _("Only execute");
    case 0:
      return _("No permission");
    default:
      g_warn_if_reached();
      return "";
    }
  } else {
    switch (p.to_int() & mask.to_int()) {
    case 07:
      return _("List directory, access, create and remove files");
    case 06:
      return _("List directory, no access to files");
    case 05:
      return _("List directory and access files");
    case 04:
      return _("List directory only, no access to files");
    case 03:
      return _("No list directory but access, create and remove");
    case 02:
      return _("No permission");
    case 01:
      return _("No list directory and access files");
    case 0:
      return _("No permission");
    default:
      g_warn_if_reached();
      return "";
    }
  }
}

static void add_to_list(GListStore *store, const char *name,
                        const char *value) {
  g_list_store_append(store, nautilus_properties_item_new(name, value));
}

NautilusPropertiesModel *eiciel_acl_model_new(const char *file_name_path) {

  try {
    eiciel::ACLManager acl_manager(file_name_path);

    auto access_users = acl_manager.get_acl_user();
    auto access_groups = acl_manager.get_acl_group();
    auto default_users = acl_manager.get_acl_user_default();
    auto default_groups = acl_manager.get_acl_group_default();

    // If tere is nothing ACL specific do not both showing anything.
    if (access_users.empty() && access_groups.empty() &&
        default_users.empty() && default_groups.empty() &&
        !acl_manager.has_mask() && !acl_manager.has_default_user() &&
        !acl_manager.has_default_group() && !acl_manager.has_default_other() &&
        !acl_manager.has_default_mask())
      return nullptr;

    GListStore *acl_model =
        (GListStore *)g_list_store_new(NAUTILUS_TYPE_PROPERTIES_ITEM);

    add_to_list(acl_model, _("User owner"),
                permissions_to_name(acl_manager.is_directory(),
                                    acl_manager.get_user()));

    eiciel::permissions_t mask = acl_manager.has_mask()
                                     ? acl_manager.get_mask()
                                     : eiciel::permissions_t(7);

    add_to_list(acl_model, _("Group owner"),
                permissions_to_name(acl_manager.is_directory(),
                                    acl_manager.get_group(), mask));

    for (const auto &entry : access_users) {
      add_to_list(acl_model,
                  g_strdup_printf(_("User ACL: ‘%s’"), entry.name.c_str()),
                  permissions_to_name(acl_manager.is_directory(), entry, mask));
    }

    for (const auto &entry : access_groups) {
      add_to_list(acl_model,
                  g_strdup_printf(_("Group ACL: ‘%s’"), entry.name.c_str()),
                  permissions_to_name(acl_manager.is_directory(), entry, mask));
    }

    if (acl_manager.has_mask()) {
      add_to_list(acl_model,
                  _("Mask limits Group owner, User ACL and Group ACL to"),
                  permissions_to_name(acl_manager.is_directory(), mask));
    }

    add_to_list(acl_model, _("Other users"),
                permissions_to_name(acl_manager.is_directory(),
                                    acl_manager.get_other()));

    if (acl_manager.is_directory()) {
      if (acl_manager.has_default_user()) {
        add_to_list(acl_model, _("New files will have User owner"),
                    permissions_to_name(acl_manager.is_directory(),
                                        acl_manager.get_user_default()));
      }

      auto default_mask = acl_manager.has_default_mask()
                              ? acl_manager.get_mask_default()
                              : eiciel::permissions_t(7);

      if (acl_manager.has_default_group()) {
        add_to_list(acl_model, _("New files will have Group owner"),
                    permissions_to_name(acl_manager.is_directory(),
                                        acl_manager.get_group_default(),
                                        default_mask));
      }

      for (const auto &entry : default_users) {
        add_to_list(acl_model,
                    g_strdup_printf(_("New files will have User ACL: ‘%s’"),
                                    entry.name.c_str()),
                    permissions_to_name(acl_manager.is_directory(), entry,
                                        default_mask));
      }

      for (const auto &entry : default_groups) {
        add_to_list(acl_model,
                    g_strdup_printf(_("New files will have Group ACL: ‘%s‘"),
                                    entry.name.c_str()),
                    permissions_to_name(acl_manager.is_directory(), entry,
                                        default_mask));
      }

      if (acl_manager.has_default_mask()) {
        add_to_list(acl_model, _("New files will have Mask"),
                    permissions_to_name(acl_manager.is_directory(),
                                        acl_manager.get_mask_default()));
      }
      if (acl_manager.has_default_other()) {
        add_to_list(acl_model, _("New files will have Other users"),
                    permissions_to_name(acl_manager.is_directory(),
                                        acl_manager.get_other_default()));
      }
    }

    return nautilus_properties_model_new(_("Access Control List"),
                                         (GListModel *)acl_model);

  } catch (...) {
    // Catch all to avoid crashing nautilus.
  }
  return nullptr;
}
