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

#include "eiciel_app_window.h"
#include <stdexcept>

EicielAppWindow::EicielAppWindow(BaseObjectType *cobject,
                                 const Glib::RefPtr<Gtk::Builder> &refBuilder)
    : Gtk::ApplicationWindow(cobject), m_refBuilder(refBuilder) {}

// static
EicielAppWindow *EicielAppWindow::create() {
  // Load the Builder file and instantiate its widgets.
  auto refBuilder = Gtk::Builder::create_from_resource(
      "/org/roger_ferrer/eiciel/app_window.ui");

  auto window = Gtk::Builder::get_widget_derived<EicielAppWindow>(refBuilder,
                                                                  "app_window");
  if (!window)
    throw std::runtime_error("No \"app_window\" object app_in app_window.ui");

  return window;
}

void EicielAppWindow::open_file_view(
    const Glib::RefPtr<Gio::File> & /* file */) {}