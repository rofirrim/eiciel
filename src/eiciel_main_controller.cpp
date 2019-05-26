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
#include "config.hpp"
#include <grp.h>
#include <iostream>
#include <pwd.h>

#include "acl_element_kind.hpp"
#include "eiciel_main_controller.hpp"

EicielMainController::EicielMainController()
    : _ACL_manager(NULL)
    , _is_file_opened(false)
    , _last_error_message("")
    , _list_must_be_updated(true)
    , _show_system(false)
{
    fill_lists();
}

EicielMainController::~EicielMainController()
{
    delete _ACL_manager;
}

void EicielMainController::fill_lists()
{
    if (!_list_must_be_updated)
        return;

    // Create the list of users
    _users_list.clear();
    struct passwd* u;
    setpwent();
    while ((u = getpwent()) != NULL) {
        if (_show_system || (u->pw_uid >= 1000)) {
            _users_list.insert(u->pw_name);
        }
    }
    endpwent();

    // Create the list of groups
    _groups_list.clear();
    struct group* g;
    setgrent();
    while ((g = getgrent()) != NULL) {
        if (_show_system || (g->gr_gid >= 1000)) {
            _groups_list.insert(g->gr_name);
        }
    }
    endgrent();

    _list_must_be_updated = false;
}

void EicielMainController::show_system_participants(bool b)
{
    if (b != _show_system) {
        _show_system = b;
        _list_must_be_updated = true;
    }
}

void EicielMainController::open_file(const std::string& s)
{
    try {
        ACLManager* new_manager;
        new_manager = new ACLManager(s);

        delete _ACL_manager;
        _ACL_manager = new_manager;

        redraw_acl_list();
        _window->set_filename(s);
        _window->set_active(true);

        // This checks if file can be edited
        check_editable();

        _is_file_opened = true;
    } catch (ACLManagerException e) {
        _is_file_opened = false;
        _window->set_active(false);
        _window->empty_acl_list();
        _last_error_message = e.getMessage();
    }
}

void EicielMainController::update_acl_list()
{
    permissions_t effective_permissions(7);
    if (_ACL_manager->has_mask()) {
        effective_permissions = _ACL_manager->get_mask();
    }
    permissions_t effective_default_permissions(7);
    if (_ACL_manager->has_default_mask()) {
        effective_default_permissions = _ACL_manager->get_mask_default();
    }

    _window->update_acl_ineffective(effective_permissions, effective_default_permissions);
}

void EicielMainController::redraw_acl_list()
{
    _updating_window = true;

    Glib::RefPtr<Gtk::ListStore> ref_acl_list = _window->create_acl_list_store();

    permissions_t perms = _ACL_manager->get_user();
    std::vector<acl_entry> vACL;
    _window->add_non_selectable(
        ref_acl_list,
        Glib::locale_to_utf8(_ACL_manager->get_owner_name()), perms.reading,
        perms.writing, perms.execution, EK_USER);

    vACL = _ACL_manager->get_acl_user();
    for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
         i++) {
        _window->add_selectable(
            ref_acl_list,
            Glib::locale_to_utf8(i->name), i->reading, i->writing, i->execution,
            EK_ACL_USER);
    }

    perms = _ACL_manager->get_group();
    _window->add_non_selectable(
        ref_acl_list,
        Glib::locale_to_utf8(_ACL_manager->get_group_name()), perms.reading,
        perms.writing, perms.execution, EK_GROUP);

    vACL = _ACL_manager->get_acl_group();
    for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
         i++) {
        _window->add_selectable(
            ref_acl_list,
            Glib::locale_to_utf8(i->name), i->reading, i->writing, i->execution,
            EK_ACL_GROUP);
    }

    if (_ACL_manager->has_mask()) {
        perms = _ACL_manager->get_mask();
        _window->add_non_selectable(
            ref_acl_list,
            _("Mask"), perms.reading, perms.writing,
            perms.execution, EK_MASK);
    }

    perms = _ACL_manager->get_other();
    _window->add_non_selectable(
        ref_acl_list,
        _("Other"), perms.reading, perms.writing,
        perms.execution, EK_OTHERS);

    _window->enable_default_acl_button(_ACL_manager->is_directory());
    _window->there_is_default_acl(false);

    if (_ACL_manager->is_directory()) {
        bool there_is_default_acl = false;
        permissions_t effective_default_permissions(7);
        if (_ACL_manager->has_default_mask()) {
            effective_default_permissions = _ACL_manager->get_mask_default();
        }

        if (_ACL_manager->has_default_user()) {
            perms = _ACL_manager->get_user_default();
            _window->add_non_selectable(
                ref_acl_list,
                Glib::locale_to_utf8(_ACL_manager->get_owner_name()), perms.reading,
                perms.writing, perms.execution, EK_DEFAULT_USER);
            there_is_default_acl = true;
        }

        vACL = _ACL_manager->get_acl_user_default();

        there_is_default_acl |= (vACL.size() > 0);
        for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
             i++) {
            _window->add_selectable(
                ref_acl_list,
                Glib::locale_to_utf8(i->name), i->reading,
                i->writing, i->execution, EK_DEFAULT_ACL_USER);
        }

        if (_ACL_manager->has_default_group()) {
            perms = _ACL_manager->get_group_default();
            _window->add_non_selectable(
                ref_acl_list,
                Glib::locale_to_utf8(_ACL_manager->get_group_name()), perms.reading,
                perms.writing, perms.execution, EK_DEFAULT_GROUP);
            there_is_default_acl |= true;
        }

        vACL = _ACL_manager->get_acl_group_default();

        there_is_default_acl |= (vACL.size() > 0);
        for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
             i++) {
            _window->add_selectable(
                ref_acl_list,
                Glib::locale_to_utf8(i->name), i->reading,
                i->writing, i->execution, EK_DEFAULT_ACL_GROUP);
        }

        if (_ACL_manager->has_default_mask()) {
            perms = _ACL_manager->get_mask_default();
            _window->add_non_selectable(ref_acl_list, _("Default Mask"), perms.reading,
                perms.writing, perms.execution,
                EK_DEFAULT_MASK);
            there_is_default_acl |= true;
        }

        if (_ACL_manager->has_default_other()) {
            perms = _ACL_manager->get_other_default();
            _window->add_non_selectable(ref_acl_list, _("Default Other"), perms.reading,
                perms.writing, perms.execution,
                EK_DEFAULT_OTHERS);
            there_is_default_acl |= true;
        }
        _window->there_is_default_acl(there_is_default_acl);
    }

    _window->replace_acl_store(ref_acl_list);

    permissions_t effective_permissions(7);
    if (_ACL_manager->has_mask()) {
        effective_permissions = _ACL_manager->get_mask();
    }
    permissions_t effective_default_permissions(7);
    if (_ACL_manager->has_default_mask()) {
        effective_default_permissions = _ACL_manager->get_mask_default();
    }
    _window->update_acl_ineffective(effective_permissions, effective_default_permissions);

    _updating_window = false;
}

bool EicielMainController::is_directory()
{
    if (_ACL_manager == NULL)
        return false;

    return _ACL_manager->is_directory();
}

void EicielMainController::add_acl_entry(const std::string& s,
    ElementKind e,
    bool is_default)
{
    permissions_t p(7);

    if (is_default) {
        if (e == EK_ACL_USER) {
            e = EK_DEFAULT_ACL_USER;
        } else if (e == EK_ACL_GROUP) {
            e = EK_DEFAULT_ACL_GROUP;
        }
    }

    try {
        switch (e) {
        case EK_ACL_USER: {
            _ACL_manager->modify_acl_user(s, p);
            break;
        }
        case EK_ACL_GROUP: {
            _ACL_manager->modify_acl_group(s, p);
            break;
        }
        case EK_DEFAULT_ACL_USER: {
            _ACL_manager->modify_acl_default_user(s, p);
            break;
        }
        case EK_DEFAULT_ACL_GROUP: {
            _ACL_manager->modify_acl_default_group(s, p);
            break;
        }
        default:
            break;
        }

        redraw_acl_list();

        _window->choose_acl(s, e);
    } catch (ACLManagerException e) {
        Glib::ustring s = _("Could not add ACL entry: ") + e.getMessage();
        Gtk::Container* toplevel = _window->get_toplevel();
        if (toplevel == NULL || !toplevel->get_is_toplevel()) {
            Gtk::MessageDialog add_acl_message(s, false, Gtk::MESSAGE_ERROR,
                Gtk::BUTTONS_OK);
            add_acl_message.run();
        } else {
            Gtk::MessageDialog add_acl_message(*(Gtk::Window*)toplevel, s, false,
                Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
            add_acl_message.run();
        }
        _last_error_message = s;
        return;
    }
}

void EicielMainController::remove_acl(const std::string& entry_name,
    ElementKind e)
{
    bool updated = true;
    try {
        switch (e) {
        case EK_ACL_USER: {
            _ACL_manager->remove_acl_user(entry_name);
            break;
        }
        case EK_ACL_GROUP: {
            _ACL_manager->remove_acl_group(entry_name);
            break;
        }
        case EK_DEFAULT_ACL_USER: {
            _ACL_manager->remove_acl_user_default(entry_name);
            break;
        }
        case EK_DEFAULT_ACL_GROUP: {
            _ACL_manager->remove_acl_group_default(entry_name);
            break;
        }
        default:
            updated = false;
            break;
        }

        if (updated) {
            redraw_acl_list();
        }
    } catch (ACLManagerException e) {
        Glib::ustring s = _("Could not remove ACL entry: ") + e.getMessage();
        Gtk::Container* toplevel = _window->get_toplevel();
        if (toplevel == NULL || !toplevel->get_is_toplevel()) {
            Gtk::MessageDialog remove_acl_message(s, false, Gtk::MESSAGE_ERROR,
                Gtk::BUTTONS_OK);
            remove_acl_message.run();
        } else {
            Gtk::MessageDialog remove_acl_message(*(Gtk::Window*)toplevel, s, false,
                Gtk::MESSAGE_ERROR,
                Gtk::BUTTONS_OK);
            remove_acl_message.run();
        }
        _last_error_message = s;
    }
}

void EicielMainController::update_acl_entry(ElementKind e,
    const std::string& s,
    bool reading,
    bool writing,
    bool execution)
{
    permissions_t p(reading, writing, execution);

    try {
        switch (e) {
        case EK_MASK: {
            _ACL_manager->modify_mask(p);
            break;
        }
        case EK_DEFAULT_MASK: {
            _ACL_manager->modify_mask_default(p);
            break;
        }
        case EK_USER: {
            _ACL_manager->modify_owner_perms(p);
            break;
        }
        case EK_GROUP: {
            _ACL_manager->modify_group_perms(p);
            break;
        }
        case EK_OTHERS: {
            _ACL_manager->modify_others_perms(p);
            break;
        }
        case EK_DEFAULT_USER: {
            _ACL_manager->modify_owner_perms_default(p);
            break;
        }
        case EK_DEFAULT_GROUP: {
            _ACL_manager->modify_group_perms_default(p);
            break;
        }
        case EK_DEFAULT_OTHERS: {
            _ACL_manager->modify_others_perms_default(p);
            break;
        }
        case EK_ACL_USER: {
            _ACL_manager->modify_acl_user(s, p);
            break;
        }
        case EK_ACL_GROUP: {
            _ACL_manager->modify_acl_group(s, p);
            break;
        }
        case EK_DEFAULT_ACL_USER: {
            _ACL_manager->modify_acl_default_user(s, p);
            break;
        }
        case EK_DEFAULT_ACL_GROUP: {
            _ACL_manager->modify_acl_default_group(s, p);
            break;
        }
        default:
            break;
        }
        update_acl_list();
    } catch (ACLManagerException e) {
        Glib::ustring s = _("Could not modify ACL entry: ") + e.getMessage();
        Gtk::Container* toplevel = _window->get_toplevel();
        if (toplevel == NULL || !toplevel->get_is_toplevel()) {
            Gtk::MessageDialog modify_acl_message(s, false, Gtk::MESSAGE_ERROR,
                Gtk::BUTTONS_OK);
            modify_acl_message.run();
        } else {
            Gtk::MessageDialog modify_acl_message(*(Gtk::Window*)toplevel, s, false,
                Gtk::MESSAGE_ERROR,
                Gtk::BUTTONS_OK);
            modify_acl_message.run();
        }
        _last_error_message = s;
    }
}

void EicielMainController::change_default_acl()
{
    if (_updating_window)
        return;

    try {
        if (!_window->give_default_acl()) {
            Glib::ustring s(
                _("Are you sure you want to remove all ACL default entries?"));
            Gtk::Container* toplevel = _window->get_toplevel();
            int result;
            if (toplevel == NULL || !toplevel->get_is_toplevel()) {
                Gtk::MessageDialog remove_acl_message(s, false, Gtk::MESSAGE_QUESTION,
                    Gtk::BUTTONS_YES_NO);
                result = remove_acl_message.run();
            } else {
                Gtk::MessageDialog remove_acl_message(*(Gtk::Window*)toplevel, s, false,
                    Gtk::MESSAGE_QUESTION,
                    Gtk::BUTTONS_YES_NO);
                result = remove_acl_message.run();
            }
            if (result == Gtk::RESPONSE_YES) {
                _ACL_manager->clear_default_acl();
            }
        } else {
            _ACL_manager->create_default_acl();
        }
        redraw_acl_list();
    } catch (ACLManagerException e) {
        _last_error_message = e.getMessage();
    }
}

std::set<std::string> EicielMainController::get_users_list()
{
    fill_lists();
    return _users_list;
}

std::set<std::string> EicielMainController::get_groups_list()
{
    fill_lists();
    return _groups_list;
}

bool EicielMainController::opened_file()
{
    return _is_file_opened;
}

void EicielMainController::check_editable()
{
    /*
   * In Linux we should check CAP_FOWNER capability. At the moment give a
   * rough approach where whe check whether the user is the owner or root.
   *
   * I've not looked for what should be checked in FreeBSD
   */
    uid_t real_user = getuid();
    if ((real_user != 0) && (real_user != _ACL_manager->get_owner_uid())) {
        _window->set_readonly(true);
    } else {
        _window->set_readonly(false);
    }
}

Glib::ustring EicielMainController::last_error()
{
    return _last_error_message;
}

bool EicielMainController::lookup_user(const std::string& str)
{
    struct passwd* p = getpwnam(str.c_str());
    return (p != NULL);
}

bool EicielMainController::lookup_group(const std::string& str)
{
    struct group* g = getgrnam(str.c_str());
    return (g != NULL);
}
