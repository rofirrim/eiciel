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

#define MY_TRACE                                                               \
  do {                                                                         \
    g_warning("%s:%d: invoked '%s'\n", __FILE__, __LINE__, __FUNCTION__);      \
  } while (0)

static GList *eiciel_menu_provider_get_file_items(EicielMenuProvider *provider,
                                                  GList *files) {
  MY_TRACE;

  g_list_foreach(
      files,
      (GFunc)(void (*)(NautilusFileInfo *, gpointer))[](NautilusFileInfo * info,
                                                        gpointer user_data) {
        g_warning("Eiciel -> '%s' '%s'", nautilus_file_info_get_name(info),
                  nautilus_file_info_get_uri_scheme(info));
      },
      nullptr);

  if (files == nullptr || files->next != nullptr) {
    g_warning("Just one file is allowed");
    return nullptr;
  }

  if (!nautilus_file_info_can_write((NautilusFileInfo *)files->data)) {
    g_warning("We can't write this file");
    return nullptr;
  }

  if (strcmp(nautilus_file_info_get_uri_scheme((NautilusFileInfo *)files->data),
             "file") != 0) {
    g_warning("Not a file");
    return nullptr;
  }

  NautilusFileInfo *file_info = (NautilusFileInfo *)files->data;
  GFile *location = nautilus_file_info_get_location(file_info);
  char *local_file = g_file_get_path(location);
  g_object_unref(location);

  // Well, some files are local but do not have a real file behind them
  if (local_file == NULL) {
    g_warning("Not a local file we can use");
    return NULL;
  }
  g_free(local_file);

  g_warning("We can return a menu here");

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
    g_warning("Invoke ACL to '%s'!", local_file);
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
    g_warning("Invoke XAttr to '%s'!", local_file);
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
  MY_TRACE;
  return nullptr;
}

static void
eiciel_menu_provider_interface_init(NautilusMenuProviderInterface *iface) {
  iface->get_file_items =
      (decltype(iface->get_file_items))eiciel_menu_provider_get_file_items;
  iface->get_background_items = (decltype(iface->get_background_items))
      eiciel_menu_provider_get_background_items;
}

static void eiciel_menu_provider_class_init(EicielMenuProviderClass *self) {
  // Not sure what I should do here?
  MY_TRACE;
}

static void eiciel_menu_provider_init(EicielMenuProvider *self) {
  // Not sure what I should do here?
  MY_TRACE;
}
