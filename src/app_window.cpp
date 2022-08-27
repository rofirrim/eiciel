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

#include "eiciel/app_window.h"
#include "config.h"
#include "eiciel/acl_editor_controller.h"
#include "eiciel/acl_editor_widget.h"
#include "eiciel/application.h"
#include "eiciel/xattr_editor_controller.h"
#include "eiciel/xattr_editor_widget.h"
#include <gtkmm/filechoosernative.h>
#include <stdexcept>

namespace eiciel {

// static
AppWindow *AppWindow::create(Application::FromNautilus mode) {
  // Load the Builder file and instantiate its widgets.
  auto refBuilder = Gtk::Builder::create_from_resource(
      "/org/roger_ferrer/eiciel/app_window.ui");

  ACLEditorController *acl_editor_controller = new ACLEditorController();
  XAttrEditorController *xattr_editor_controller = new XAttrEditorController();
  auto window = Gtk::Builder::get_widget_derived<AppWindow>(
      refBuilder, "app_window", acl_editor_controller, xattr_editor_controller,
      mode);
  if (!window)
    throw std::runtime_error("No \"app_window\" object app_in app_window.ui");

  return window;
}

AppWindow::AppWindow(BaseObjectType *cobject,
                     const Glib::RefPtr<Gtk::Builder> &refBuilder,
                     ACLEditorController *acl_editor_cont,
                     XAttrEditorController *xattr_editor_cont,
                     Application::FromNautilus mode)
    : Gtk::ApplicationWindow(cobject), m_refBuilder(refBuilder),
      acl_editor_controller(acl_editor_cont),
      xattr_editor_controller(xattr_editor_cont), mode(mode) {
  auto open_file = refBuilder->get_object<Gtk::Button>("open-file");
  open_file->set_sensitive(true);
  open_file->signal_clicked().connect(
      [this]() { this->choose_file_to_open(); });

  auto open_directory = refBuilder->get_object<Gtk::Button>("open-directory");
  open_directory->set_sensitive(true);
  open_directory->signal_clicked().connect(
      [this]() { this->choose_directory_to_open(); });

  auto box_for_acl_editor =
      refBuilder->get_object<Gtk::Box>("box-for-acl-editor");
  auto acl_editor_widget =
      Gtk::make_managed<ACLEditorWidget>(acl_editor_controller);
  box_for_acl_editor->append(*acl_editor_widget);

  auto box_for_xattr_editor =
      refBuilder->get_object<Gtk::Box>("box-for-xattr-editor");
  auto xattr_editor_widget =
      Gtk::make_managed<XAttrEditorWidget>(xattr_editor_controller);
  box_for_xattr_editor->append(*xattr_editor_widget);

  filename_label = refBuilder->get_object<Gtk::Label>("filename-label");

  Glib::RefPtr<Gtk::Stack> main_stack =
      refBuilder->get_object<Gtk::Stack>("main-stack");
  Glib::RefPtr<Gtk::StackSwitcher> stack_switcher =
      refBuilder->get_object<Gtk::StackSwitcher>("stack-switcher");

  if (mode != Application::FromNautilus::NONE) {
    open_file->set_visible(false);
    open_directory->set_visible(false);
    filename_label->set_visible(false);
    stack_switcher->set_visible(false);

    if (mode == Application::FromNautilus::ACL) {
      main_stack->get_pages()->select_item(0, true);
    } else if (mode == Application::FromNautilus::XATTR) {
      main_stack->get_pages()->select_item(1, true);
    }
  }
}

void AppWindow::choose_file_impl(const Glib::ustring &title,
                                 Gtk::FileChooser::Action action) {
  auto dialog = Gtk::FileChooserNative::create(title, *this, action);

  dialog->set_modal(true);
  dialog->set_transient_for(*this);

  dialog->signal_response().connect([dialog, this](int response_id) mutable {
    switch (response_id) {
    case Gtk::ResponseType::ACCEPT: {
      open_file(dialog->get_file());
      break;
    }
    default: {
      break;
    }
    }
    dialog.reset();
  });

  dialog->show();
}

void AppWindow::choose_file_to_open() {
  choose_file_impl(_("Select file"), Gtk::FileChooser::Action::OPEN);
}

void AppWindow::choose_directory_to_open() {
  choose_file_impl(_("Select directory"),
                   Gtk::FileChooser::Action::SELECT_FOLDER);
}

void AppWindow::open_file(const Glib::RefPtr<Gio::File> &file) {
  bool result = false;
  if (file->get_uri_scheme() == "file") {
    acl_editor_controller->open_file(file->get_path());
    result = acl_editor_controller->opened_file();
    xattr_editor_controller->open_file(file->get_path());
    result = result || acl_editor_controller->opened_file();
  }
  if (!result) {
    filename_label->set_text(_("No file opened"));
  } else {
    filename_label->set_text(file->get_basename());
  }
}

} // namespace eiciel
