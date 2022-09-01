/*
     - GNOME editor of ACL file permissions.
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
#include "eiciel/app_window.h"
#include "eiciel/acl_list_widget.h"
#include "eiciel/participant_list_widget.h"
#include <gtkmm/aboutdialog.h>
#include <exception>
#include <iostream>

namespace eiciel {

Application::Application()
    : Gtk::Application("org.roger_ferrer.Eiciel",
                       Gio::Application::Flags::HANDLES_COMMAND_LINE |
                           Gio::Application::Flags::HANDLES_OPEN) {}

#define EDIT_MODE_FLAG "edit-mode"
#define EDIT_MODE_FLAG_SHORT 'm'

Glib::RefPtr<Application> Application::create() {
  auto app = Glib::make_refptr_for_instance<Application>(new Application());

  app->add_main_option_entry(Gio::Application::OptionType::STRING,
                             EDIT_MODE_FLAG, EDIT_MODE_FLAG_SHORT,
                             "Initial edit mode (default otherwise)",
                             "acl|xattr", Glib::OptionEntry::Flags::HIDDEN);

  app->signal_command_line().connect(
      [app](const Glib::RefPtr<Gio::ApplicationCommandLine> &cmdline) -> int {
        Glib::RefPtr<Glib::VariantDict> options_dict =
            cmdline->get_options_dict();
        Glib::ustring edit_mode;
        if (options_dict->contains(EDIT_MODE_FLAG)) {
          options_dict->lookup_value(EDIT_MODE_FLAG, edit_mode);
          if (edit_mode != "acl" && edit_mode != "xattr") {
            Glib::ustring error_message = "Invalid value for " EDIT_MODE_FLAG
                                          " option. Valid options are "
                                          "'acl' or 'xattr'\n";
            cmdline->printerr(error_message);
            return EXIT_FAILURE;
          }
        }

        int argc;
        char **argv = cmdline->get_arguments(argc);

        if (argc <= 1) {
          app->activate();
        } else {
          for (int i = 1; i < argc; i++) {
            Glib::RefPtr<Gio::File> file =
                cmdline->create_file_for_arg(argv[i]);
            app->open(file, edit_mode);
          }
        }

        return EXIT_SUCCESS;
      },
      /* after */ false);

  Glib::RefPtr<Gio::SimpleAction> about_action = app->add_action("about");

  about_action->signal_activate().connect([app](const Glib::VariantBase &) {
    Gtk::Window *win = app->get_active_window();
    if (!win)
      return;

    auto about = new Gtk::AboutDialog();

    about->set_program_name("Eiciel");
    Glib::RefPtr<Gdk::Texture> logo = Gdk::Texture::create_from_resource(
        "/org/roger_ferrer/eiciel/icon_eiciel_128.png");
    about->set_logo(logo);

    about->set_version(VERSION);
    about->set_copyright("Copyright 2004-2022 - Roger Ferrer Ibáñez");
    about->set_comments(_("Graphical editor of file ACLs and extended attributes"));
    about->set_website_label("https://rofi.roger-ferrer.org/eiciel");
    about->set_website("https://rofi.roger-ferrer.org/eiciel");

    about->set_translator_credits(_("translator-credits"));
    about->set_license_type(Gtk::License::GPL_2_0);
    about->set_hide_on_close();

    about->signal_hide().connect(
        [about, logo]() mutable {
          if (logo) {
            logo->unreference();
            logo = nullptr;
          }
          delete about;
          about = nullptr;
        },
        false);

    about->set_modal();
    about->set_transient_for(*win);
    about->present();
  });

  Glib::RefPtr<Gio::SimpleAction> quit_action = app->add_action("quit");

  quit_action->signal_activate().connect(
      [app](const Glib::VariantBase &) { app->quit(); });

  return app;
}

AppWindow *Application::create_appwindow(EditMode mode) {
  auto appwindow = AppWindow::create(mode);

  // Make sure that the application runs for as long this window is still open.
  add_window(*appwindow);

  // A window can be added to an application with Gtk::Application::add_window()
  // or Gtk::Window::set_application(). When all added windows have been hidden
  // or removed, the application stops running (Gtk::Application::run()
  // returns()), unless Gio::Application::hold() has been called.

  // Delete the window when it is hidden.
  appwindow->signal_hide().connect(sigc::bind(
      sigc::mem_fun(*this, &Application::on_hide_window), appwindow));

  return appwindow;
}

void Application::on_activate() {

  try {
    // The application has been started, so let's show a window.
    auto appwindow = create_appwindow(EditMode::DEFAULT);
    appwindow->present();
  }
  // If create_appwindow() throws an exception (perhaps from Gtk::Builder),
  // no window has been created, no window has been added to the application,
  // and therefore the application will stop running.
  catch (const Glib::Error &ex) {
    std::cerr << "Application::on_activate(): " << ex.what() << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << "Application::on_activate(): " << ex.what() << std::endl;
  }
}

void Application::on_startup() {
  Gtk::Application::on_startup();
}

void Application::on_open(const Gio::Application::type_vec_files &files,
                                const Glib::ustring & hint) {
  // The application has been asked to open some files,
  // so let's open a new view for each one.
  AppWindow *appwindow = nullptr;

  g_return_if_fail(hint.empty() || hint == "acl" || hint == "xattr");

  EditMode mode = EditMode::DEFAULT;
  if (!hint.empty()) {
    mode = hint == "acl" ? EditMode::ACL : EditMode::XATTR;
  }

  try {
    appwindow = create_appwindow(mode);

    for (const auto &file : files) {
      appwindow->open_file(file);
      break;
    }

    appwindow->present();
  } catch (const Glib::Error &ex) {
    std::cerr << "Application::on_open(): " << ex.what() << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << "Application::on_open(): " << ex.what() << std::endl;
  }
}

void Application::on_hide_window(Gtk::Window *window) { delete window; }

}
