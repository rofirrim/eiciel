/*
    Eiciel - GNOME editor of Participant file permissions.
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

#ifndef EICIEL_CONFIRM_TOGGLE_BUTTON_H
#define EICIEL_CONFIRM_TOGGLE_BUTTON_H

#include <gtkmm/togglebutton.h>
#include <gtkmm/builder.h>

namespace eiciel {

class ConfirmToggleButton final : public Gtk::ToggleButton {
public:
  ConfirmToggleButton(BaseObjectType *obj,
                      const Glib::RefPtr<Gtk::Builder> &builder)
      : Glib::ObjectBase("ConfirmToggleButton"), Gtk::ToggleButton(obj) {}

  sigc::signal<void(bool activate)> signal_toggle_requested;

private:
  void on_clicked() override {
    bool new_state = !get_active();
    signal_toggle_requested.emit(new_state);
  }
};
} // namespace eiciel

#endif
