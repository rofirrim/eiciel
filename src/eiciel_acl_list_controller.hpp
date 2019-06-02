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

#ifndef EICIEL_ACL_LIST_CONTROLLER_HPP
#define EICIEL_ACL_LIST_CONTROLLER_HPP

#include "acl_element_kind.hpp"
#include "acl_manager.hpp"
#include <gtkmm.h>
#include <string>

class EicielACLList;

class EicielACLListController {
public:
    virtual ~EicielACLListController();

    void update_acl_ineffective(permissions_t effective_permissions, permissions_t effective_default_permissions);
    Glib::RefPtr<Gtk::ListStore> create_acl_list_store();
    void empty_acl_list();
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
    void can_edit_default_acl(bool b);
    void default_acl_are_being_edited(bool b);
    void replace_acl_store(Glib::RefPtr<Gtk::ListStore> ref_acl_list);
    void choose_acl(const std::string& s, ElementKind e);
    void set_readonly(bool b);
    void set_active(bool b);

    void insert_user(const std::string& s);
    void insert_group(const std::string& s);
    void insert_default_user(const std::string& s);
    void insert_default_group(const std::string& s);
    void remove_entry(const std::string &s, ElementKind e);
    void populate_required_default_entries();
    void remove_all_default_entries();

    void get_textual_representation(std::string& access_acl, std::string& default_acl);

    permissions_t get_mask_permissions();
    permissions_t get_default_mask_permissions();

    // Events
    virtual bool toggle_edit_default_acl(bool default_acl_were_being_edited) = 0;
    virtual void remove_acl(const std::string& entry_name, ElementKind e) = 0;
    virtual void update_acl_entry(ElementKind e,
        const std::string& name,
        bool reading,
        bool writing,
        bool execution) = 0;

    void set_view(EicielACLList *view) { _view = view; }
private:
    EicielACLList* _view;
};

#endif
