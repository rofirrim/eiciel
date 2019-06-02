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

#include "eiciel_acl_list_controller.hpp"
#include "eiciel_acl_list.hpp"

EicielACLListController ::~EicielACLListController() {}

void EicielACLListController::update_acl_ineffective(permissions_t effective_permissions, permissions_t effective_default_permissions)
{
    return _view->update_acl_ineffective(effective_permissions, effective_default_permissions);
}

Glib::RefPtr<Gtk::ListStore> EicielACLListController::create_acl_list_store()
{
    return _view->create_acl_list_store();
}

void EicielACLListController::empty_acl_list()
{
    return _view->empty_acl_list();
}

void EicielACLListController::add_non_selectable(
    Glib::RefPtr<Gtk::ListStore> ref_acl_list,
    Glib::ustring title,
    bool reading,
    bool writing,
    bool execution,
    ElementKind e)
{
    return _view->add_non_selectable(ref_acl_list, title, reading, writing, execution, e);
}

void EicielACLListController::add_selectable(
    Glib::RefPtr<Gtk::ListStore> ref_acl_list,
    Glib::ustring title,
    bool reading,
    bool writing,
    bool execution,
    ElementKind e)
{
    return _view->add_selectable(ref_acl_list, title, reading, writing, execution, e);
}

void EicielACLListController::can_edit_default_acl(bool b)
{
    return _view->can_edit_default_acl(b);
}

void EicielACLListController::default_acl_are_being_edited(bool b)
{
    return _view->default_acl_are_being_edited(b);
}

void EicielACLListController::replace_acl_store(Glib::RefPtr<Gtk::ListStore> ref_acl_list)
{
    return _view->replace_acl_store(ref_acl_list);
}

void EicielACLListController::choose_acl(const std::string& s, ElementKind e)
{
    return _view->choose_acl(s, e);
}

void EicielACLListController::set_readonly(bool b)
{
    return _view->set_readonly(b);
}

void EicielACLListController::set_active(bool b)
{
    return _view->set_active(b);
}

void EicielACLListController::insert_user(const std::string& s)
{
    return _view->insert_user(s);
}

void EicielACLListController::insert_group(const std::string& s)
{
    return _view->insert_group(s);
}

void EicielACLListController::insert_default_user(const std::string& s)
{
    return _view->insert_default_user(s);
}

void EicielACLListController::insert_default_group(const std::string& s)
{
    return _view->insert_default_group(s);
}

void EicielACLListController::remove_entry(const std::string& s, ElementKind e)
{
    return _view->remove_entry(s, e);
}

void EicielACLListController::remove_all_default_entries()
{
    return _view->remove_all_default_entries();
}

void EicielACLListController::populate_required_default_entries()
{
    return _view->populate_required_default_entries();
}

void EicielACLListController::get_textual_representation(std::string &access_acl, std::string& default_acl)
{
    return _view->get_textual_representation(access_acl, default_acl);
}

permissions_t EicielACLListController::get_mask_permissions() {
    return _view->get_mask_permissions();
}

permissions_t EicielACLListController::get_default_mask_permissions() {
    return _view->get_default_mask_permissions();
}
