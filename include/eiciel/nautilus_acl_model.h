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

#ifndef EICIEL_NAUTILUS_ACL_MODEL_H
#define EICIEL_NAUTILUS_ACL_MODEL_H

// This seems unnecessary but glib-typeof.h uses C++, so make sure we pull that
// stuff first in before we include nautilus-extension.h
#include <glib-object.h>
extern "C" {
#include <nautilus-extension.h>
}

G_BEGIN_DECLS

NautilusPropertiesModel *eiciel_acl_model_new(const char * file_path);

G_END_DECLS

#endif // EICIEL_NAUTILUS_ACL_MODEL_H
