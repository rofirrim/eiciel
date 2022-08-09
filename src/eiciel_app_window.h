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

#ifndef EICIEL_APP_WINDOW_HPP
#define EICIEL_APP_WINDOW_HPP

#include <gtkmm.h>

class EicielAppWindow : public Gtk::ApplicationWindow {
public:
  EicielAppWindow();

  EicielAppWindow(BaseObjectType *cobject,
                  const Glib::RefPtr<Gtk::Builder> &refBuilder);

  static EicielAppWindow *create();

  void open_file_view(const Glib::RefPtr<Gio::File> &file);

protected:
  Glib::RefPtr<Gtk::Builder> m_refBuilder;
};

#endif /* EICIEL_APP_WINDOW_HPP */
