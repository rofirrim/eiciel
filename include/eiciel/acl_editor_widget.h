
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,  USA
*/
#ifndef EICIEL_MAIN_WINDOW_HPP
#define EICIEL_MAIN_WINDOW_HPP

#include "config.h"

#include "eiciel/i18n.h"

#include "eiciel/acl_element_kind.h"
#include "eiciel/acl_list_widget.h"
#include "eiciel/participant_list_widget.h"
#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/infobar.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <set>
#include <string>

namespace eiciel {

class ACLEditorController;

class ACLEditorWidget : public Gtk::Box {
public:
  ACLEditorWidget();
  ACLEditorWidget(BaseObjectType *obj,
                  const Glib::RefPtr<Gtk::Builder> &builder,
                  ACLEditorController *cont);
  virtual ~ACLEditorWidget();

  void initialize(const Glib::ustring &s);
  bool opened_file();

  void set_active(bool b);

  Glib::ustring last_error();

  void show_info_bar();
  void hide_info_bar();

  void can_edit_enclosed_files(bool b);
  void set_recursive_progress(double);

  static GType get_type() {
    // Let's cache once the type does exist.
    if (!gtype)
      gtype = g_type_from_name("gtkmm__CustomObject_ACLEditorWidget");
    return gtype;
  }

private:
    ACLEditorController* controller;
    Gtk::InfoBar* info_bar;
    Gtk::Label* info_bar_label;
    Gtk::ProgressBar* info_bar_progress;
    Gtk::Box* main_box;
    Gtk::Button* edit_enclosed_files;

    void there_is_no_file();
    void do_edit_enclosed_files();

    static GType gtype;
};

}

#endif
