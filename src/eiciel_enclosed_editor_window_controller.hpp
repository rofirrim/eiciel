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

#ifndef EICIEL_ENCLOSED_EDITOR_WINDOW_CONTROLLER_HPP
#define EICIEL_ENCLOSED_EDITOR_WINDOW_CONTROLLER_HPP

#include "eiciel_acl_list_controller.hpp"
#include "eiciel_enclosed_editor_window.hpp"
#include "eiciel_participant_list_controller.hpp"

class EicielEnclosedEditorController {
public:
    EicielEnclosedEditorController();

    EicielACLListController* get_acl_list_directory_controller() { return &_acl_list_directory_controller; }
    EicielACLListController* get_acl_list_file_controller() { return &_acl_list_file_controller; }
    EicielParticipantListController* get_participant_list_controller() { return &_participant_list_controller; }

    std::string get_directory_access_acl_text() { return _directory_access_acl_text; }
    std::string get_directory_default_acl_text() { return _directory_default_acl_text; }
    std::string get_file_access_acl_text() { return _file_access_acl_text; }

private:
    class ACLListDirectoryController : public EicielACLListController {
    public:
        ACLListDirectoryController(EicielEnclosedEditorController* editor_controller)
            : _editor_controller(editor_controller)
        {
        }

        virtual ~ACLListDirectoryController() {}

        bool toggle_edit_default_acl(bool default_acl_were_being_edited) override;
        void remove_acl(const std::string& entry_name, ElementKind e) override;
        void update_acl_entry(ElementKind e,
            const std::string& name,
            bool reading,
            bool writing,
            bool execution)
            override;

    private:
        EicielEnclosedEditorController* _editor_controller;
    };

    class ACLListFileController : public EicielACLListController {
    public:
        ACLListFileController(EicielEnclosedEditorController* editor_controller)
            : _editor_controller(editor_controller)
        {
        }

        virtual ~ACLListFileController() {}

        bool toggle_edit_default_acl(bool default_acl_were_being_edited) override;
        void remove_acl(const std::string& entry_name, ElementKind e) override;
        void update_acl_entry(ElementKind e,
            const std::string& name,
            bool reading,
            bool writing,
            bool execution)
            override;

    private:
        EicielEnclosedEditorController* _editor_controller;
    };

    class ParticipantListController : public EicielParticipantListController {
    public:
        ParticipantListController(EicielEnclosedEditorController* editor_controller)
            : _editor_controller(editor_controller)
        {
        }
        virtual ~ParticipantListController() {}

        void add_acl_entry(AddParticipantTarget target, const std::string& s, ElementKind e, bool is_default) override;

    private:
        EicielEnclosedEditorController* _editor_controller;
    };

    ACLListDirectoryController _acl_list_directory_controller;
    ACLListFileController _acl_list_file_controller;
    ParticipantListController _participant_list_controller;
    EicielEnclosedEditor* _window;

    std::string _directory_access_acl_text;
    std::string _directory_default_acl_text;
    std::string _file_access_acl_text;

    void apply_changes();

    friend class EicielEnclosedEditor;
};

#endif // EICIEL_ENCLOSED_EDITOR_WINDOW_CONTROLLER_HPP
