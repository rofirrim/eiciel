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

#ifndef EICIEL_XATTR_WINDOW_HPP
#define EICIEL_XATTR_WINDOW_HPP

#include <config.hpp>
#include <gtkmm.h>
#include <glib/gi18n-lib.h>
#include <sstream>
#include "xattr_manager.hpp"
#include "xattr_list_model.hpp"
#include "eiciel_xattr_controller.hpp"

class EicielXAttrController;

class EicielXAttrWindow : public Gtk::VBox
{
	public:
		EicielXAttrWindow(EicielXAttrController* _controller);
		virtual ~EicielXAttrWindow();

		void fill_attributes(XAttrManager::attributes_t llista);
		void set_readonly(bool b);

		void set_active(bool b);

	private:
		EicielXAttrController* _controller;

        Glib::RefPtr<Gtk::ListStore> _ref_xattr_list;
		XAttrListModel _xattr_list_model;

		Gtk::ScrolledWindow _xattr_listview_container;
		Gtk::TreeView _xattr_listview;

		Gtk::Button _b_add_attribute;
		Gtk::Button _b_remove_attribute;

		Gtk::HBox _bottom_buttonbox;

		void _xattr_selection_change();

		void there_is_no_xattr_selection();
		void there_is_xattr_selection();

		void set_name_edited_attribute(const Glib::ustring& vell, const Glib::ustring& nou);
		void set_value_edited_attribute(const Glib::ustring& vell, const Glib::ustring& nou);

		void remove_selected_attribute();
		void add_selected_attribute();

		bool _readonly;
}
;

#endif
