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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,  USA
   */

#ifndef EICIEL_ENCLOSED_EDITOR_WINDOW_HPP
#define EICIEL_ENCLOSED_EDITOR_WINDOW_HPP

#include "config.h"
#include <gtkmm/window.h>
#include <gtkmm/dialog.h>
#include <gtkmm/builder.h>
#include "eiciel/acl_list_widget.h"
#include "eiciel/participant_list_widget.h"

namespace eiciel {

class EnclosedACLEditorController;

class EnclosedACLEditorWidget : public Gtk::Dialog {
public:
  EnclosedACLEditorWidget(BaseObjectType *cobject,
                          const Glib::RefPtr<Gtk::Builder> &refBuilder,
                          EnclosedACLEditorController *controller);
  virtual ~EnclosedACLEditorWidget();

  static EnclosedACLEditorWidget *create(EnclosedACLEditorController *);

  ACLListWidget *get_files_acl_list_widget() const { return files_acl_list; }
  ACLListWidget *get_dirs_acl_list_widget() const { return dirs_acl_list; }
  ParticipantListWidget *get_participant_list_widget() const {
    return participants_list;
  }

private:
  EnclosedACLEditorController *controller;
  Glib::RefPtr<Gtk::Builder> m_refBuilder;

  ACLListWidget *files_acl_list;
  ACLListWidget *dirs_acl_list;
  ParticipantListWidget *participants_list;

  Gtk::Button *apply_button;

  void apply_changes();
};
}

#endif // EICIEL_ENCLOSED_EDITOR_WINDOW_HPP
