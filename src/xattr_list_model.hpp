/*
   Eiciel - GNOME editor of ACL file permissions.
   Copyright (C) 2004-2014 Roger Ferrer Ibáñez

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
#ifndef LLISTA_XATTR_HPP
#define LLISTA_XATTR_HPP

#include <config.hpp>
#include <gtkmm.h>

class XAttrListModel : public Gtk::TreeModel::ColumnRecord
{
    public:
        XAttrListModel()
        {
            add(_attribute_name);
            add(_attribute_value);
        }

        Gtk::TreeModelColumn<Glib::ustring> _attribute_name;
        Gtk::TreeModelColumn<Glib::ustring> _attribute_value;
};

#endif
