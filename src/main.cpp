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
#include "eiciel/application.h"
#include <iostream>

int main(int argc, char *argv[]) {
  Glib::RefPtr<eiciel::Application> app = eiciel::Application::create();

  app->add_main_option_entry(Gio::Application::OptionType::STRING,
                             "from-nautilus", 'f',
                             "Used by the Nautilus integration", "acl|xattr",
                             Glib::OptionEntry::Flags::HIDDEN);

  app->signal_handle_local_options().connect(
      [app](const Glib::RefPtr<Glib::VariantDict> &variant_dict) -> int {
        if (variant_dict->contains("from-nautilus")) {
          Glib::ustring from_nautilus;
          variant_dict->lookup_value("from-nautilus", from_nautilus);
          g_return_val_if_fail(
              from_nautilus == "acl" || from_nautilus == "xattr", 1);
          app->set_from_nautilus(
              from_nautilus == "acl"
                  ? eiciel::Application::FromNautilus::ACL
                  : eiciel::Application::FromNautilus::XATTR);
        }
        return -1;
      },
      /* after */ true);
  return app->run(argc, argv);
}
