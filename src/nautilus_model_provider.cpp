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

#include "eiciel/nautilus_model_provider.h"
#include "eiciel/nautilus_acl_model.h"
#include "eiciel/nautilus_xattr_model.h"
extern "C" {
#include <nautilus-extension.h>
}

static void eiciel_model_provider_interface_init(
    NautilusPropertiesModelProviderInterface *iface);

struct _EicielModelProvider {
  GObject parent_instance;

  /* Other members, including private data. */
};

G_DEFINE_TYPE_WITH_CODE(
    EicielModelProvider, eiciel_model_provider, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(NAUTILUS_TYPE_PROPERTIES_MODEL_PROVIDER,
                          eiciel_model_provider_interface_init))

static GList *eiciel_model_get_models(EicielModelProvider *provider,
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

  GList *result = nullptr;
  if (NautilusPropertiesModel *acl_model = eiciel_acl_model_new(local_file))
    result = g_list_append(result, acl_model);
  if (NautilusPropertiesModel *xattr_model = eiciel_xattr_model_new(local_file))
    result = g_list_append(result, xattr_model);

  return result;
}

static void eiciel_model_provider_interface_init(
    NautilusPropertiesModelProviderInterface *iface) {
  iface->get_models = (decltype(iface->get_models))eiciel_model_get_models;
}

static void eiciel_model_provider_class_init(EicielModelProviderClass *self) {
}

static void eiciel_model_provider_init(EicielModelProvider *self) {
}
