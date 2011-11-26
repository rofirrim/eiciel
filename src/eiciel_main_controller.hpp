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
#ifndef EICIEL_MAIN_CONTROLER_HPP
#define EICIEL_MAIN_CONTROLER_HPP

#include <config.hpp>
#include <set>
#include <glib/gi18n-lib.h>
#include "eiciel_main_window.hpp"
#include "acl_manager.hpp"
#include "acl_element_kind.hpp"

using namespace std;

class EicielWindow;

class EicielMainController : public sigc::trackable
{
	private:
		ACLManager* _ACL_manager;
		EicielWindow* _window;
        set<string> _users_list;
        set<string> _groups_list;
		bool _is_file_opened;
		bool _updating_window;
		Glib::ustring _last_error_message;

		bool _list_must_be_updated;
		bool _show_system;

		void update_acl_list();
		void update_acl_entry(ElementKind e, string nom, 
				bool lectura, bool escriptura, bool execucio);
		void remove_acl(string nomEntrada, ElementKind e);
		void add_acl_entry(string s, ElementKind e, bool esDefault);
		void change_default_acl();

		void fill_lists();
		void show_system_participants(bool b);
		void check_editable();

        set<string> get_users_list();
        set<string> get_groups_list();

        bool is_directory();
	public:
		void open_file(string s);
		Glib::ustring last_error();
		bool opened_file();
		friend class EicielWindow; 
		EicielMainController();
		~EicielMainController();
};

#endif
