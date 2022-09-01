/*
    - GNOME editor of ACL file permissions.
   Copyright (C) 2019-2022 Roger Ferrer Ibáñez

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

#include "eiciel/i18n.h"
#include <locale.h>
#include <libintl.h>

namespace eiciel {

void enable_i18n() {
  setlocale(LC_ALL, "");
  bindtextdomain("eiciel", LOCALEDIR);
  textdomain("eiciel");

  // We want translations in UTF-8
  bind_textdomain_codeset("eiciel", "UTF-8");
}
} // namespace eiciel
