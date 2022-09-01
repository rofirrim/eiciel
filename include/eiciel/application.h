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

#ifndef EICIEL_APPLICATION_H
#define EICIEL_APPLICATION_H

#include "config.h"

#include "eiciel/i18n.h"

#include <giomm/application.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>
#include <gtkmm/application.h>
#include <gtkmm/window.h>

namespace eiciel {

class AppWindow;

class Application : public Gtk::Application {
protected:
  Application();

public:
  static Glib::RefPtr<Application> create();

  enum class EditMode { DEFAULT, ACL, XATTR };

protected:
  // Override default signal handlers:
  void on_activate() override;
  void on_startup() override;
  void on_open(const Gio::Application::type_vec_files &files,
               const Glib::ustring &hint) override;

private:
  AppWindow *create_appwindow(EditMode edit_mode);
  void on_hide_window(Gtk::Window *window);
};

} // namespace eiciel

#endif // EICIEL_APPLICATION_H
