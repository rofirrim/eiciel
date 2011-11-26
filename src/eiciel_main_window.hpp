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
#ifndef EICIEL_MAIN_WINDOW_HPP
#define EICIEL_MAIN_WINDOW_HPP

#include <config.hpp>
#include <string>
#include <gtkmm.h>
#include <glib/gi18n-lib.h>
#include "acl_list.hpp"
#include "participant_list.hpp"
#include "eiciel_main_controller.hpp"
#include "acl_element_kind.hpp"

using namespace std;

class EicielMainController;

class EicielWindow : public Gtk::VBox 
{
	public:
		EicielWindow(EicielMainController* cont);
		virtual ~EicielWindow();

		void add_non_selectable(Glib::ustring titol, bool lectura, bool escriptura, bool execucio, ElementKind e,
				bool efectiuLectura = true, bool efectiuEscriptura = true, bool efectiuExecucio = true);
		void add_selectable(Glib::ustring titol, bool lectura, bool escriptura, bool execucio, ElementKind e,
				bool efectiuLectura = true, bool efectiuEscriptura = true, bool efectiuExecucio = true);
		void empty_acl_list();
		void set_filename(string filename);
		void enable_default_acl_button(bool b);
		void there_is_default_acl(bool b);
		bool give_default_acl();

        void initialize(string s);
		bool opened_file();

		void set_active(bool b);

		void set_readonly(bool b);

		void show_exclamation_mark(bool b);
		void choose_acl(string s, ElementKind e);
		void toggle_system_show();

		Glib::ustring last_error();

	private:
		Gtk::VBox _main_area;
        
		Glib::RefPtr<Gtk::ListStore> _ref_acl_list;
		Gtk::ScrolledWindow _listview_acl_container;
		Gtk::TreeView _listview_acl;
        
        Glib::RefPtr<Gtk::ListStore> _ref_participants_list;
        Gtk::ScrolledWindow _listview_participants_container;
        Gtk::TreeView _listview_participants;
        
        Gtk::HBox _participant_chooser;
		Gtk::RadioButton _rb_acl_user;
		Gtk::RadioButton _rb_acl_group;
        Gtk::CheckButton _cb_acl_default;
		Gtk::ToggleButton _tb_modify_default_acl;
		Gtk::CheckButton _cb_show_system_participants;

		Gtk::HBox _middle_button_group;
		Gtk::Button _b_add_acl;
		Gtk::Button _b_remove_acl;

		Glib::RefPtr<Gdk::Pixbuf> _user_icon;
		Glib::RefPtr<Gdk::Pixbuf> _group_icon;
		Glib::RefPtr<Gdk::Pixbuf> _others_icon;
		Glib::RefPtr<Gdk::Pixbuf> _mask_icon;

		Glib::RefPtr<Gdk::Pixbuf> _user_icon_acl;
		Glib::RefPtr<Gdk::Pixbuf> _group_icon_acl;

		Glib::RefPtr<Gdk::Pixbuf> _default_user_icon;
		Glib::RefPtr<Gdk::Pixbuf> _default_group_icon;
		Glib::RefPtr<Gdk::Pixbuf> _default_others_icon;
		Glib::RefPtr<Gdk::Pixbuf> _default_user_icon_acl;
		Glib::RefPtr<Gdk::Pixbuf> _default_group_icon_acl;
		Glib::RefPtr<Gdk::Pixbuf> _default_mask_icon;
		
		Gtk::Frame _top_frame;
		Gtk::VBox _top_area;
		Gtk::Frame _bottom_frame;
		Gtk::VBox _bottom_area;
		
        Gtk::Image _warning_icon;
		Gtk::Label _bottom_label;

		ACLListModel _acl_list_model;
        ModelLlistaParticipant _participant_list_model;

        RecursionPolicyModel _recursion_policy_model;
        Glib::RefPtr<Gtk::ListStore> _recursion_policy_list;
        Glib::RefPtr<Gtk::ListStore> _dummy_recursion_policy_list;

		Glib::RefPtr<Gtk::UIManager> _ref_ui_manager;

		bool _readonly_mode;

		EicielMainController* _controller;

		set<string> _users_list;
		set<string> _groups_list;


		void there_is_no_file();
		void acl_selection_change();
		void there_is_acl_selection();
		void there_is_no_acl_selection();

		void change_participant_selection();
		void there_is_participant_selection();
		void there_is_no_participant_selection();
		Glib::RefPtr<Gdk::Pixbuf> get_proper_icon(ElementKind e);
		void add_element(Glib::ustring title, bool reading, bool writing, bool execution,
				ElementKind e, Gtk::TreeModel::Row& row,
				bool effective_reading, bool effective_writing, bool effective_execution,
                bool can_be_recursed);
		void remove_selected_acl();
		void add_selected_participant();
		void change_permissions(const Glib::ustring& cadena, PermissionKind p);

		void fill_participants(set<string>* participants,
				ElementKind tipus, 
				Glib::RefPtr<Gdk::Pixbuf> iconaNormal,
				Glib::RefPtr<Gdk::Pixbuf> iconaDefecte);


		void participants_list_double_click(const Gtk::TreeModel::Path& p, Gtk::TreeViewColumn* c);
		void acl_list_double_click(const Gtk::TreeModel::Path& p, Gtk::TreeViewColumn* c);


		bool acl_selection_function(const Glib::RefPtr<Gtk::TreeModel>& model,
				const Gtk::TreeModel::Path& path, bool);

        void acl_cell_data_func(Gtk::CellRenderer*, const Gtk::TreeModel::iterator&);

		void set_value_drag_and_drop(const Glib::RefPtr<Gdk::DragContext>&, 
				Gtk::SelectionData& selection_data, guint, guint);
		void get_value_drag_and_drop(const Glib::RefPtr<Gdk::DragContext>& context, 
				int, int, const Gtk::SelectionData& selection_data, guint, guint time);

		void start_drag_and_drop(const Glib::RefPtr<Gdk::DragContext>&);

		void change_participant_kind();

        void recursion_policy_change(const Glib::ustring& path_string, const Glib::ustring& new_text);
        
};

#endif
