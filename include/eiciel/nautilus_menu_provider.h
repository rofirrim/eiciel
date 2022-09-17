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

#ifndef EICIEL_NAUTILUS_MENU_PROVIDER_H
#define EICIEL_NAUTILUS_MENU_PROVIDER_H

#include "config.h"

#include "eiciel/i18n.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define EICIEL_TYPE_MENU_PROVIDER (eiciel_menu_provider_get_type())

G_DECLARE_FINAL_TYPE(EicielMenuProvider, eiciel_menu_provider, EICIEL,
                     MENU_PROVIDER, GObject)

G_END_DECLS

#endif // EICIEL_NAUTILUS_MENU_PROVIDER_H
