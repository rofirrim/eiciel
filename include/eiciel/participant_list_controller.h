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

#ifndef EICIEL_PARTICIPANT_LIST_CONTROLLER_H
#define EICIEL_PARTICIPANT_LIST_CONTROLLER_H

#include "eiciel/acl_element_kind.h"
#include "eiciel/participant_target.h"
#include "eiciel/participant_list_widget.h"
#include <glibmm/ustring.h>
#include <set>
#include <string>

namespace eiciel {

class ParticipantListWidget;

class ParticipantListController {
public:

    ParticipantListController();
    virtual ~ParticipantListController();

    void set_readonly(bool b);
    void set_active(bool b);
    void can_edit_default_acl(bool b);

    std::set<Glib::ustring> get_users_list();
    std::set<Glib::ustring> get_groups_list();

    void show_system_participants(bool b);

    bool lookup_user(const Glib::ustring& str);
    bool lookup_group(const Glib::ustring& str);

    void set_view(ParticipantListWidget* v) { view = v; }

    // Events
    virtual void add_acl_entry(AddParticipantTarget target, const Glib::ustring& s, ElementKind e, bool is_default) = 0;

private:
    bool list_must_be_updated;
    bool show_system;
    std::set<Glib::ustring> users_list;
    std::set<Glib::ustring> groups_list;

    ParticipantListWidget* view;

    void fill_lists();
};

}

#endif // EICIEL_PARTICIPANT_LIST_CONTROLLER_H

