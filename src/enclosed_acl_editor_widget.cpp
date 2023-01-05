/*
    - GNOME editor of ACL file permissions.
   Copyright (C) 2019 Roger Ferrer Ibáñez

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

#include "eiciel/enclosed_acl_editor_widget.h"
#include "eiciel/enclosed_acl_editor_controller.h"

namespace eiciel {

EnclosedACLEditorWidget *
EnclosedACLEditorWidget::create(EnclosedACLEditorController *controller) {
  auto refBuilder = Gtk::Builder::create_from_resource(
      "/org/roger_ferrer/eiciel/enclosed_acl_editor_widget.ui");

  auto window = Gtk::Builder::get_widget_derived<EnclosedACLEditorWidget>(
      refBuilder, "enclosed-acl-editor-dialog", controller);

  g_return_val_if_fail(window, nullptr);

  return window;
}

EnclosedACLEditorWidget::EnclosedACLEditorWidget(
    BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &refBuilder,
    EnclosedACLEditorController *controller)
    : Glib::ObjectBase("EnclosedACLEditorWidget"), Gtk::Dialog(cobject),
      controller(controller) {
  controller->set_view(this);

  auto files_acl_list_controller = controller->get_acl_list_file_controller();
  files_acl_list = Gtk::Builder::get_widget_derived<ACLListWidget>(
      refBuilder, "files-acl-list-widget", files_acl_list_controller,
      ACLListWidgetMode::ONLY_FILE);
  files_acl_list_controller->can_edit_default_acl(false);

  dirs_acl_list = Gtk::Builder::get_widget_derived<ACLListWidget>(
      refBuilder, "dirs-acl-list-widget",
      controller->get_acl_list_directory_controller(),
      ACLListWidgetMode::ONLY_DIRECTORY);

  participants_list = Gtk::Builder::get_widget_derived<ParticipantListWidget>(
      refBuilder, "participant-list-widget",
      controller->get_participant_list_controller(),
      ParticipantListWidgetMode::DUAL_PANE);

  apply_button = refBuilder->get_widget<Gtk::Button>("apply-button");
}

EnclosedACLEditorWidget::~EnclosedACLEditorWidget() {}

} // namespace eiciel
