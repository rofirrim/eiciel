/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2004-2022 Roger Ferrer Ibáñez

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
#ifndef EICIEL_MAIN_WINDOW_CONTROLLER_HPP
#define EICIEL_MAIN_WINDOW_CONTROLLER_HPP

#include "config.h"

#include "eiciel/i18n.h"

#include "eiciel/acl_editor_widget.h"
#include "eiciel/acl_element_kind.h"
#include "eiciel/acl_list_controller.h"
#include "eiciel/acl_manager.h"
#include "eiciel/app_window.h"
#include "eiciel/participant_list_controller.h"
#include "eiciel/participant_target.h"
#include <functional>
#include <set>

namespace eiciel {

class EnclosedACLEditorController;

class ACLEditorController : public sigc::trackable,
                            public ACLListController,
                            public ParticipantListController {
public:
  void open_file(const Glib::ustring &s);
  bool opened_file();

  ACLEditorController();
  ~ACLEditorController();

  void set_active(bool);

  // ACLListController
  void requested_toggle_edit_default_acl(bool requested_new_state) override;
  void confirmed_toggle_edit_default_acl(bool new_state);
  bool remove_acl(const Glib::ustring &entry_name, ElementKind e) override;
  void update_acl_entry(ElementKind e, const Glib::ustring &name, bool reading,
                        bool writing, bool execution) override;
  void edit_enclosed_files();

  // EicielParticipantListController
  void add_acl_entry(AddParticipantTarget target, const Glib::ustring &s,
                     ElementKind e, bool is_default) override;

  void set_view(ACLEditorWidget *v) { view = v; }


private:
  ACLManager *acl_manager;
  ACLEditorWidget *view;
  bool is_file_opened;
  Glib::ustring current_filename;

  void update_acl_list();
  void redraw_acl_list();

  void check_editable();

  bool is_directory();

  void set_readonly(bool b);

  void can_edit_enclosed_files(bool b);

  void show_message_dialog_error(const Glib::ustring &s);

  void fill_acl_list(bool include_default_entries);

  void apply_recursively(EnclosedACLEditorController *, Gtk::Window *toplevel);
};

}

#endif
