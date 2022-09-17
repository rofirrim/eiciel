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

#include "config.h"

#include "eiciel/i18n.h"

#include "eiciel/nautilus_menu_provider.h"
#include "eiciel/nautilus_model_provider.h"
extern "C" {
#include <nautilus-extension.h>
}

static GType type_list[2];

void nautilus_module_initialize(GTypeModule *module) {
  g_debug("Initializing Eiciel extension");
  eiciel::enable_i18n();
  type_list[0] = eiciel_menu_provider_register_in_module(module);
  type_list[1] = eiciel_model_provider_register_in_module(module);
}
/**
 * nautilus_module_shutdown: (skip)
 *
 * Called when the extension is being unloaded.
 */
void nautilus_module_shutdown(void) {
  g_debug("Shutting down Eiciel extension");
}
/**
 * nautilus_module_list_types: (skip)
 * @types: (out) (transfer none) (array length=num_types): array of GType *
 * @num_types: the number of types in the array
 *
 * Called after the extension has been initialized and has registered all the
 * types it exports, to load them into Nautilus.
 */
void nautilus_module_list_types(const GType **types, int *num_types) {

  *num_types = sizeof(type_list) / sizeof(*type_list);
  *types = type_list;
}
