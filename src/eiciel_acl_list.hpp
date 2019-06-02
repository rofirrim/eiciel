/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2019 Roger Ferrer Ibáñez

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
#ifndef EICIEL_ACL_LIST_HPP
#define EICIEL_ACL_LIST_HPP

#include "acl_element_kind.hpp"
#include "acl_list.hpp"
#include "acl_manager.hpp"
#include "participant_list.hpp"
#include <config.hpp>
#include <glib/gi18n-lib.h>
#include <gtkmm.h>
#include <set>
#include <string>

class EicielACLListController;

class EicielACLList : public Gtk::Box {
public:
    EicielACLList(EicielACLListController* cont);
    virtual ~EicielACLList();

    void disable_default_acl_editing();

private:
    /* GUI components */
    /*   */ Gtk::Box _main_box;
    /*      |- */ Gtk::ScrolledWindow _listview_acl_container;
    /*      |     |- */ Gtk::TreeView _listview_acl;
    /*      |- */ Gtk::Box _warning_hbox;
    /*      |     |- */ Gtk::Image _warning_icon;
    /*      |     |- */ Gtk::Label _warning_label;
    /*      |- */ Gtk::Box _middle_button_group;
    /*            |- */ Gtk::Button _b_remove_acl;
    /*            |- */ Gtk::CheckButton _cb_modify_default_acl;

    /* Non GUI */
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

    Glib::RefPtr<Gtk::ListStore> _ref_acl_list;

    ACLListModel _acl_list_model;

    bool _readonly_mode;
    bool _toggling_default_acl;

    EicielACLListController* _controller;

    void acl_selection_change();
    void there_is_acl_selection();
    void there_is_no_acl_selection();

    Glib::RefPtr<Gdk::Pixbuf> get_proper_icon(ElementKind e);
    void add_element(Glib::ustring title,
        bool reading,
        bool writing,
        bool execution,
        ElementKind e,
        Gtk::TreeModel::Row& row);
    void remove_selected_acl();
    void change_permissions(const Glib::ustring& str, PermissionKind p);

    bool acl_selection_function(const Glib::RefPtr<Gtk::TreeModel>& model,
        const Gtk::TreeModel::Path& path,
        bool);

    void update_acl_ineffective(permissions_t effective_permissions, permissions_t effective_default_permissions);
    void acl_cell_data_func(Gtk::CellRenderer*, const Gtk::TreeModel::iterator&);

    void add_non_selectable(
        Glib::RefPtr<Gtk::ListStore> ref_acl_list,
        Glib::ustring title,
        bool reading,
        bool writing,
        bool execution,
        ElementKind e);
    void add_selectable(
        Glib::RefPtr<Gtk::ListStore> ref_acl_list,
        Glib::ustring title,
        bool reading,
        bool writing,
        bool execution,
        ElementKind e);
    void empty_acl_list();
    void default_acl_are_being_edited(bool b);
    void can_edit_default_acl(bool b);

    void set_active(bool b);
    void set_readonly(bool b);

    void set_exist_ineffective_permissions(bool b);
    void choose_acl(const std::string& s, ElementKind e);

    Glib::RefPtr<Gtk::ListStore> create_acl_list_store();
    Glib::RefPtr<Gtk::ListStore> get_acl_store();
    void replace_acl_store(Glib::RefPtr<Gtk::ListStore> ref_acl_list);

    void toggle_edit_default_acl();

    void populate_required_nondefault_entries();
    void populate_required_default_entries();
    void remove_all_default_entries();
    bool nondefault_acl_is_empty();
    void remove_unneeded_entries();

    void insert_before(const std::string& s, ElementKind new_element_kind, ElementKind before_element_kind, bool selectable);
    void insert_user(const std::string& s);
    void insert_group(const std::string& s);
    void insert_default_user(const std::string& s);
    void insert_default_group(const std::string& s);
    void remove_entry(const std::string& s, ElementKind e);

    void get_textual_representation(std::string& access_acl, std::string &default_acl);

    permissions_t get_mask_permissions();
    permissions_t get_default_mask_permissions();

    friend class EicielACLListController;
};

#endif
