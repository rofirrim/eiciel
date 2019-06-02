/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2004-2019 Roger Ferrer Ibáñez

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
#ifndef ACL_LIST_HPP
#define ACL_LIST_HPP
#include "acl_element_kind.hpp"
#include "cellrenderer_acl.hpp"
#include <config.hpp>

class ACLListModel : public Gtk::TreeModel::ColumnRecord {
public:
    ACLListModel()
    {
        add(_icon);
        add(_entry_name);
        add(_reading_permission);
        add(_writing_permission);
        add(_execution_permission);
        add(_removable);
        add(_entry_kind);
        add(_reading_ineffective);
        add(_writing_ineffective);
        add(_execution_ineffective);
        add(_empty);
    }

    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> _icon;
    Gtk::TreeModelColumn<Glib::ustring> _entry_name;
    Gtk::TreeModelColumn<bool> _reading_permission;
    Gtk::TreeModelColumn<bool> _writing_permission;
    Gtk::TreeModelColumn<bool> _execution_permission;
    Gtk::TreeModelColumn<bool> _removable;
    Gtk::TreeModelColumn<ElementKind> _entry_kind;
    Gtk::TreeModelColumn<bool> _reading_ineffective;
    Gtk::TreeModelColumn<bool> _writing_ineffective;
    Gtk::TreeModelColumn<bool> _execution_ineffective;
    Gtk::TreeModelColumn<Glib::ustring> _empty;
};

#endif
