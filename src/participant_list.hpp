/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2004-2010 Roger Ferrer Ibáñez

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
#ifndef LLISTA_PARTICIPANT_HPP
#define LLISTA_PARTICIPANT_HPP
#include <config.hpp>

#include "acl_element_kind.hpp"

class ModelLlistaParticipant : public Gtk::TreeModel::ColumnRecord
{
	public:
		ModelLlistaParticipant()
		{
			add(_icon);
			add(_participant_name);
            add(_entry_kind);
		}
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > _icon;
		Gtk::TreeModelColumn<Glib::ustring> _participant_name;
        Gtk::TreeModelColumn<ElementKind> _entry_kind;
};

#endif
