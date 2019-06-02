/*
    Eiciel - GNOME editor of Participant file permissions.
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

#include "eiciel_participant_list_controller.hpp"
#include "eiciel_participant_list.hpp"
#include <grp.h>
#include <pwd.h>

EicielParticipantListController::EicielParticipantListController()
    : _list_must_be_updated(true), _show_system(false)
{
    fill_lists();
}

EicielParticipantListController::~EicielParticipantListController() {}

void EicielParticipantListController::can_edit_default_acl(bool b)
{
    _view->can_edit_default_acl(b);
}

void EicielParticipantListController::set_readonly(bool b)
{
    _view->set_readonly(b);
}

void EicielParticipantListController::set_active(bool b)
{
    set_readonly(!b);
}

void EicielParticipantListController::fill_lists()
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

std::set<std::string> EicielParticipantListController::get_users_list()
{
    fill_lists();
    return _users_list;
}

std::set<std::string> EicielParticipantListController::get_groups_list()
{
    fill_lists();
    return _groups_list;
}

void EicielParticipantListController::show_system_participants(bool b)
{
    if (b != _show_system) {
        _show_system = b;
        _list_must_be_updated = true;
    }
}

bool EicielParticipantListController::lookup_user(const std::string& str)
{
    struct passwd* p = getpwnam(str.c_str());
    return (p != NULL);
}

bool EicielParticipantListController::lookup_group(const std::string& str)
{
    struct group* g = getgrnam(str.c_str());
    return (g != NULL);
}

