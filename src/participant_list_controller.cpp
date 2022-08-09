/*
    Eiciel - GNOME editor of Participant file permissions.
    Copyright (C) 2019-2022 Roger Ferrer Ibáñez

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

#include "eiciel/participant_list_controller.h"
#include "eiciel/participant_item.h"
#include <grp.h>
#include <pwd.h>

namespace eiciel {

ParticipantListController::ParticipantListController()
    : list_must_be_updated(true), show_system(false)
{
    fill_lists();
}

ParticipantListController::~ParticipantListController() {}

void ParticipantListController::can_edit_default_acl(bool b)
{
    view->can_edit_default_acl(b);
}

void ParticipantListController::set_readonly(bool b)
{
    view->set_readonly(b);
}

void ParticipantListController::set_active(bool b)
{
    set_readonly(!b);
}

void ParticipantListController::fill_lists()
{
    if (!list_must_be_updated)
        return;

    // Create the list of users
    users_list.clear();
    struct passwd* u;
    setpwent();
    while ((u = getpwent()) != NULL) {
        if (show_system || (u->pw_uid >= 1000)) {
            users_list.insert(u->pw_name);
        }
    }
    endpwent();

    // Create the list of groups
    groups_list.clear();
    struct group* g;
    setgrent();
    while ((g = getgrent()) != NULL) {
        if (show_system || (g->gr_gid >= 1000)) {
            groups_list.insert(g->gr_name);
        }
    }
    endgrent();

    list_must_be_updated = false;
}

std::set<Glib::ustring> ParticipantListController::get_users_list()
{
    fill_lists();
    return users_list;
}

std::set<Glib::ustring> ParticipantListController::get_groups_list()
{
    fill_lists();
    return groups_list;
}

void ParticipantListController::show_system_participants(bool b)
{
    if (b != show_system) {
        show_system = b;
        list_must_be_updated = true;
    }
}

bool ParticipantListController::lookup_user(const Glib::ustring& str)
{
    struct passwd* p = getpwnam(str.c_str());
    return (p != NULL);
}

bool ParticipantListController::lookup_group(const Glib::ustring& str)
{
    struct group* g = getgrnam(str.c_str());
    return (g != NULL);
}

}