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

#ifndef EICIEL_APP_WINDOW_HPP
#define EICIEL_APP_WINDOW_HPP

#include "config.h"

#include "eiciel/i18n.h"

#include "eiciel/acl_editor_controller.h"
#include "eiciel/application.h"
#include "eiciel/xattr_editor_controller.h"
#include <giomm/file.h>
#include <glibmm/refptr.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>

namespace eiciel {

class AppWindow : public Gtk::ApplicationWindow {
public:
  AppWindow();

  AppWindow(BaseObjectType *cobject,
            const Glib::RefPtr<Gtk::Builder> &refBuilder,
            ACLEditorController *acl_editor_cont,
            XAttrEditorController *xattr_editor_cont,
            Application::EditMode mode);

  static AppWindow *create(Application::EditMode mode);

  void choose_file_to_open();
  void choose_directory_to_open();
  void open_file(const Glib::RefPtr<Gio::File> &file);

protected:
  void choose_file_impl(const Glib::ustring &title,
                        Gtk::FileChooser::Action action);

  Glib::RefPtr<Gtk::Builder> m_refBuilder;
  ACLEditorController *acl_editor_controller;
  XAttrEditorController *xattr_editor_controller;
  Glib::RefPtr<Gtk::Label> filename_label;
};

} // namespace eiciel

#endif /* EICIEL_APP_WINDOW_HPP */
