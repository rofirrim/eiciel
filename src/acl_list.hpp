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
#ifndef LLISTA_ACL_HPP
#define LLISTA_ACL_HPP
#include <config.hpp>
#include "acl_element_kind.hpp"
#include "cellrenderer_acl.hpp"

class ACLListModel : public Gtk::TreeModel::ColumnRecord
{
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
            add(_current_recursion_policy);
            // TODO - Implement recursion policies
#if 0
            add(_recursion_policies);
#endif
            add(_empty);
        }

        Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > _icon;
        Gtk::TreeModelColumn<Glib::ustring> _entry_name;
        Gtk::TreeModelColumn<bool> _reading_permission;
        Gtk::TreeModelColumn<bool> _writing_permission;
        Gtk::TreeModelColumn<bool> _execution_permission;
        Gtk::TreeModelColumn<bool> _removable;
        Gtk::TreeModelColumn<ElementKind> _entry_kind;
        Gtk::TreeModelColumn<bool> _reading_ineffective;
        Gtk::TreeModelColumn<bool> _writing_ineffective;
        Gtk::TreeModelColumn<bool> _execution_ineffective;
        Gtk::TreeModelColumn<Glib::ustring> _current_recursion_policy;
        // Gtk::TreeModelColumn<Glib::RefPtr<Gtk::TreeModel> > _recursion_policies;
        Gtk::TreeModelColumn<Glib::ustring> _empty;
};

class RecursionPolicyModel : public Gtk::TreeModel::ColumnRecord
{
    public:
        RecursionPolicyModel()
        { 
            add(_recursion_policy); 
        }

        // The values from which the user may choose.
        Gtk::TreeModelColumn<Glib::ustring> _recursion_policy; 
};

#endif
