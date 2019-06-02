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

#ifndef EICIEL_PARTICIPANT_LIST_CONTROLLER_HPP
#define EICIEL_PARTICIPANT_LIST_CONTROLLER_HPP

#include "acl_element_kind.hpp"
#include "eiciel_participant_target.hpp"
#include <set>
#include <string>

class EicielParticipantList;

class EicielParticipantListController {
public:

    EicielParticipantListController();
    virtual ~EicielParticipantListController();

    void set_readonly(bool b);
    void set_active(bool b);
    void can_edit_default_acl(bool b);

    std::set<std::string> get_users_list();
    std::set<std::string> get_groups_list();

    void show_system_participants(bool b);

    bool lookup_user(const std::string& str);
    bool lookup_group(const std::string& str);

    void set_view(EicielParticipantList* view) { _view = view; }

    // Events
    virtual void add_acl_entry(AddParticipantTarget target, const std::string& s, ElementKind e, bool is_default) = 0;

private:
    bool _list_must_be_updated;
    bool _show_system;
    std::set<std::string> _users_list;
    std::set<std::string> _groups_list;

    EicielParticipantList* _view;

    void fill_lists();
};

#endif

