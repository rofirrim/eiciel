/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2004-2022 Roger Ferrer Ibáñez

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
#ifndef EICIEL_ACL_ELEMENT_KIND_H
#define EICIEL_ACL_ELEMENT_KIND_H

#include "config.h"

#include "eiciel/i18n.h"

namespace eiciel {

enum class ElementKind {
  user,
  group,
  others,

  acl_user,
  acl_group,
  mask,

  default_user,
  default_group,
  default_others,

  default_acl_user,
  default_acl_group,
  default_mask
};

enum class PermissionKind { reading, writing, execution };

} // namespace eiciel

#endif // EICIEL_ACL_ELEMENT_KIND
