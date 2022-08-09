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

#include "eiciel_application.h"
#include "eiciel_app_window.h"
#include <exception>
#include <iostream>

EicielApplication::EicielApplication()
    : Gtk::Application("org.roger_ferrer.Eiciel",
                       Gio::Application::Flags::HANDLES_OPEN) {}

Glib::RefPtr<EicielApplication> EicielApplication::create() {
  return Glib::make_refptr_for_instance<EicielApplication>(
      new EicielApplication());
}

EicielAppWindow *EicielApplication::create_appwindow() {
  auto appwindow = EicielAppWindow::create();

  // Make sure that the application runs for as long this window is still open.
  add_window(*appwindow);

  // A window can be added to an application with Gtk::Application::add_window()
  // or Gtk::Window::set_application(). When all added windows have been hidden
  // or removed, the application stops running (Gtk::Application::run()
  // returns()), unless Gio::Application::hold() has been called.

  // Delete the window when it is hidden.
  appwindow->signal_hide().connect(sigc::bind(
      sigc::mem_fun(*this, &EicielApplication::on_hide_window), appwindow));

  return appwindow;
}

void EicielApplication::on_activate() {
  try {
    // The application has been started, so let's show a window.
    auto appwindow = create_appwindow();
    appwindow->present();
  }
  // If create_appwindow() throws an exception (perhaps from Gtk::Builder),
  // no window has been created, no window has been added to the application,
  // and therefore the application will stop running.
  catch (const Glib::Error &ex) {
    std::cerr << "EicielApplication::on_activate(): " << ex.what() << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << "EicielApplication::on_activate(): " << ex.what() << std::endl;
  }
}

void EicielApplication::on_open(const Gio::Application::type_vec_files &files,
                                const Glib::ustring & /* hint */) {
  // The application has been asked to open some files,
  // so let's open a new view for each one.
  EicielAppWindow *appwindow = nullptr;
  auto windows = get_windows();
  if (windows.size() > 0)
    appwindow = dynamic_cast<EicielAppWindow *>(windows[0]);

  try {
    if (!appwindow)
      appwindow = create_appwindow();

    for (const auto &file : files)
      appwindow->open_file_view(file);

    appwindow->present();
  } catch (const Glib::Error &ex) {
    std::cerr << "EicielApplication::on_open(): " << ex.what() << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << "EicielApplication::on_open(): " << ex.what() << std::endl;
  }
}

void EicielApplication::on_hide_window(Gtk::Window *window) { delete window; }