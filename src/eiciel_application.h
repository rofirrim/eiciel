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
#include <gtkmm.h>

class EicielAppWindow;

class EicielApplication : public Gtk::Application {
protected:
  EicielApplication();

public:
  static Glib::RefPtr<EicielApplication> create();

protected:
  // Override default signal handlers:
  void on_activate() override;
  void on_open(const Gio::Application::type_vec_files &files,
               const Glib::ustring &hint) override;

private:
  EicielAppWindow *create_appwindow();
  void on_hide_window(Gtk::Window *window);
};
