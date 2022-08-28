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

#include "eiciel/nautilus_menu_provider.h"
extern "C" {
#include <nautilus-extension.h>
}

static void
eiciel_menu_provider_interface_init(NautilusMenuProviderInterface *iface);

struct _EicielMenuProvider {
  GObject parent_instance;

  /* Other members, including private data. */
};

G_DEFINE_TYPE_WITH_CODE(
    EicielMenuProvider, eiciel_menu_provider, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(NAUTILUS_TYPE_MENU_PROVIDER,
                          eiciel_menu_provider_interface_init))

static GList *eiciel_menu_provider_get_file_items(EicielMenuProvider *provider,
                                                  GList *files) {
  if (files == nullptr || files->next != nullptr) {
    return nullptr;
  }

  NautilusFileInfo *file_info = (NautilusFileInfo *)files->data;

  if (!nautilus_file_info_can_write(file_info)) {
    return nullptr;
  }

  if (strcmp(nautilus_file_info_get_uri_scheme(file_info), "file") != 0) {
    return nullptr;
  }

  GFile *location = nautilus_file_info_get_location(file_info);
  char *local_file = g_file_get_path(location);
  g_object_unref(location);

  // Well, some files are local but do not have a real file behind them
  if (local_file == NULL) {
    return NULL;
  }
  g_free(local_file);

  GList *result = nullptr;
  NautilusMenuItem *new_menu_item;
  new_menu_item = nautilus_menu_item_new(
      "access_control_list", "Edit Access Control Lists…",
      "Allows editing Access Control Lists", nullptr);

  // ACL
  auto cb_acl = [](NautilusMenuItem *instance, NautilusFileInfo *file_info) {
    GFile *location = nautilus_file_info_get_location(file_info);
    char *local_file = g_file_get_path(location);
    g_return_if_fail(local_file);
    g_object_unref(location);

    gchar* quoted_local_file = g_shell_quote(local_file);

    GString *cmd = g_string_new("eiciel");
    g_string_append(cmd, " --edit-mode=acl ");
    g_string_append(cmd, quoted_local_file);

    g_print("EXEC: %s\n", cmd->str);
    g_spawn_command_line_async(cmd->str, NULL);

    g_string_free(cmd, TRUE);
    g_free(quoted_local_file);
    g_free(local_file);
  };
  g_signal_connect_object(
      new_menu_item, "activate",
      (GCallback)(void (*)(NautilusMenuItem *, NautilusFileInfo *))cb_acl,
      file_info, (GConnectFlags)0);
  result = g_list_append(result, new_menu_item);

  // XAttr
  new_menu_item =
      nautilus_menu_item_new("extended_attributes", "Edit extended attributes…",
                             "Allows editing Access Control Lists", nullptr);
  auto cb_xattr = [](NautilusMenuItem *instance, NautilusFileInfo *file_info) {
    GFile *location = nautilus_file_info_get_location(file_info);
    char *local_file = g_file_get_path(location);
    g_return_if_fail(local_file);
    g_object_unref(location);

    gchar* quoted_local_file = g_shell_quote(local_file);

    GString *cmd = g_string_new("eiciel");
    g_string_append(cmd, " --edit-mode=xattr ");
    g_string_append(cmd, quoted_local_file);
    
    g_print ("EXEC: %s\n", cmd->str);
    g_spawn_command_line_async (cmd->str, NULL);

    g_string_free(cmd, TRUE);
    g_free(quoted_local_file);
    g_free(local_file);
  };
  g_signal_connect_object(
      new_menu_item, "activate",
      (GCallback)(void (*)(NautilusMenuItem *, NautilusFileInfo *))cb_xattr,
      file_info, (GConnectFlags)0);
  result = g_list_append(result, new_menu_item);
  return result;
}

GList *
eiciel_menu_provider_get_background_items(EicielMenuProvider *provider,
                                          NautilusFileInfo *current_folder) {
  return nullptr;
}

static void
eiciel_menu_provider_interface_init(NautilusMenuProviderInterface *iface) {
  iface->get_file_items =
      (decltype(iface->get_file_items))eiciel_menu_provider_get_file_items;
  iface->get_background_items = (decltype(iface->get_background_items))
      eiciel_menu_provider_get_background_items;
}

static void eiciel_menu_provider_class_init(EicielMenuProviderClass *self) {}

static void eiciel_menu_provider_init(EicielMenuProvider *self) {}
