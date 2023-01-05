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

#ifndef EICIEL_ENCLOSED_ACL_EDITOR_CONTROLLER_HPP
#define EICIEL_ENCLOSED_ACL_EDITOR_CONTROLLER_HPP

#include "eiciel/acl_list_controller.h"
#include "eiciel/enclosed_acl_editor_widget.h"
#include "eiciel/participant_list_controller.h"

namespace eiciel {

class EnclosedACLEditorWidget;

class EnclosedACLEditorController {
private:
  class ACLListDirectoryController : public ACLListController {
  public:
    ACLListDirectoryController(EnclosedACLEditorController *controller)
        : controller(controller) {}

    virtual ~ACLListDirectoryController() {}

    void requested_toggle_edit_default_acl(bool default_acl_were_being_edited) override;
    void confirmed_toggle_edit_default_acl(bool new_state);
    bool remove_acl(const Glib::ustring &entry_name, ElementKind e) override;
    void update_acl_entry(ElementKind e, const Glib::ustring &name,
                          bool reading, bool writing, bool execution) override;

  private:
    EnclosedACLEditorController *controller;
  };

  class ACLListFileController : public ACLListController {
  public:
    ACLListFileController(EnclosedACLEditorController *controller)
        : controller(controller) {}

    virtual ~ACLListFileController() {}

    void requested_toggle_edit_default_acl(bool default_acl_were_being_edited) override;
    bool remove_acl(const Glib::ustring &entry_name, ElementKind e) override;
    void update_acl_entry(ElementKind e, const Glib::ustring &name,
                          bool reading, bool writing, bool execution) override;

  private:
    EnclosedACLEditorController *controller;
  };

  class EnclosedParticipantListController : public ParticipantListController {
  public:
    EnclosedParticipantListController(EnclosedACLEditorController *controller)
        : controller(controller) {}
    virtual ~EnclosedParticipantListController() {}

    void add_acl_entry(AddParticipantTarget target, const Glib::ustring &s,
                       ElementKind e, bool is_default) override;

    void apply_changes();

  private:
    EnclosedACLEditorController *controller;
  };

public:
  EnclosedACLEditorController();

  ACLListController *get_acl_list_file_controller() {
    return &acl_list_file_controller;
  }
  ACLListController *get_acl_list_directory_controller() {
    return &acl_list_directory_controller;
  }
  ParticipantListController *get_participant_list_controller() {
    return &participant_list_controller;
  }

  Glib::ustring get_directory_access_acl_text() {
    return directory_access_acl_text;
  }
  Glib::ustring get_directory_default_acl_text() {
    return directory_default_acl_text;
  }
  Glib::ustring get_file_access_acl_text() { return file_access_acl_text; }

  void set_view(EnclosedACLEditorWidget *v) { view = v; }

  void compute_acl_textual_representation();

private:
  ACLListFileController acl_list_file_controller;
  ACLListDirectoryController acl_list_directory_controller;
  EnclosedParticipantListController participant_list_controller;

  Glib::ustring directory_access_acl_text;
  Glib::ustring directory_default_acl_text;
  Glib::ustring file_access_acl_text;

  EnclosedACLEditorWidget *view = nullptr;
};

} // namespace eiciel

#endif // EICIEL_ENCLOSED_ACL_EDITOR_CONTROLLER_HPP
