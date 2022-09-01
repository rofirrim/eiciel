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

#ifndef EICIEL_PARTICIPANT_LIST_WIDGET_H
#define EICIEL_PARTICIPANT_LIST_WIDGET_H

#include "config.h"

#include "eiciel/i18n.h"

#include "eiciel/participant_item.h"
#include "eiciel/participant_list_controller.h"
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/columnview.h>
#include <gtkmm/entry.h>
#include <gtkmm/filterlistmodel.h>
#include <set>
#include <unordered_map>

namespace eiciel {

class ParticipantListController;

enum class ParticipantListWidgetMode {
  SINGLE_PANE = 0,
  DUAL_PANE
};

class ParticipantListWidget : public Gtk::Box {
public:
  ParticipantListWidget(ParticipantListController *, ParticipantListWidgetMode);
  virtual ~ParticipantListWidget();

  void set_readonly(bool b);
  void can_edit_default_acl(bool b);
private:
  void fill_models(bool system_participants);

  void remove_button_signal(Gtk::Button *);
  void keep_button_signal(Gtk::Button *, sigc::connection);

  void update_default_value_model_list(Glib::RefPtr<ParticipantListItemModel> current_model);
  void update_default_value_user_list();
  void update_default_value_group_list();

  void fill_model_list(Glib::RefPtr<ParticipantListItemModel> current_model,
                       std::set<Glib::ustring> list,
                       ParticipantKind non_default_kind,
                       ParticipantKind default_kind);
  void fill_user_list();
  void fill_group_list();

  bool highlight_entry(const Glib::ustring &name,
                       Glib::RefPtr<ParticipantListItemModel> current_model);
  void do_search_participant();

  Glib::RefPtr<ParticipantListItemModel> user_model;
  Glib::RefPtr<Gtk::FilterListModel> filter_user_model;

  Glib::RefPtr<ParticipantListItemModel> group_model;
  Glib::RefPtr<Gtk::FilterListModel> filter_group_model;

  Glib::RefPtr<Gtk::ColumnView> column_view;
  Glib::RefPtr<Gtk::CheckButton> cb_acl_default;

  Glib::RefPtr<Gtk::CheckButton> cb_user;
  Glib::RefPtr<Gtk::CheckButton> cb_group;

  Glib::RefPtr<Gtk::CheckButton> cb_show_system_participants;

  Glib::RefPtr<Gtk::Entry> filter_entry;

  Glib::RefPtr<Gtk::Entry> searched_participant;
  Glib::RefPtr<Gtk::Button> search_participant;

  ParticipantListController *controller;
  std::unordered_map<Gtk::Button*, sigc::connection> button_signal_map;

  ParticipantKind participant_kind;
  ParticipantListWidgetMode widget_mode;
  bool is_default;

  gulong searched_participant_signal_activate;
};

} // namespace eiciel

#endif // EICIEL_PARTICIPANT_LIST_WIDGET_H
