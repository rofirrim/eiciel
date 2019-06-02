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
#ifndef EICIEL_PARTICIPANT_LIST_HPP
#define EICIEL_PARTICIPANT_LIST_HPP

#include "acl_element_kind.hpp"
#include "acl_list.hpp"
#include "eiciel_participant_target.hpp"
#include "participant_list.hpp"
#include <config.hpp>
#include <glib/gi18n-lib.h>
#include <gtkmm.h>
#include <set>
#include <string>

class EicielParticipantListController;

class EicielParticipantList : public Gtk::Box {
public:
    EicielParticipantList(EicielParticipantListController* cont);
    virtual ~EicielParticipantList();

    enum ParticipantListActionMode
    {
        MAIN_EDITOR = 0,
        ENCLOSED_FILES_EDITOR
    };

    // FIXME: could this be a parameter in the constructor instead?
    void set_mode(ParticipantListActionMode mode);

private:
    /* GUI components */
    /* |- */ Gtk::Box _main_box;
    /*   |- */ Gtk::Box _participant_chooser;
    /*   |     |- */ Gtk::RadioButton _rb_acl_user;
    /*   |     |- */ Gtk::RadioButton _rb_acl_group;
    /*   |     |- */ Gtk::CheckButton _cb_acl_default;
    /*   |     |- */ Gtk::Entry _filter_entry;
    /*   |- */ Gtk::ScrolledWindow _listview_participants_container;
    /*   |     |- */ Gtk::TreeView _listview_participants;
    /*   |- */ Gtk::Box _below_participant_list;
    /*   |     |- */ Gtk::Button _b_add_acl;
    /*   |     |- */ Gtk::Button _b_add_acl_directory;
    /*   |     |- */ Gtk::Button _b_add_acl_file;
    /*   |- */ Gtk::Expander _advanced_features_expander;
    /*     |- */ Gtk::Box _advanced_features_box;
    /*       |- */ Gtk::Box _participant_entry_box;
    /*       |     |- */ Gtk::Label _participant_entry_label;
    /*       |     |- */ Gtk::Entry _participant_entry;
    /*       |     |- */ Gtk::Button _participant_entry_query_button;
    /*       |- */ Gtk::CheckButton _cb_show_system_participants;

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

    Glib::RefPtr<Gtk::ListStore> _ref_participants_list;
    Glib::RefPtr<Gtk::TreeModelFilter> _ref_participants_list_filter;

    ParticipantListModel _participant_list_model;

    int _pending_filter_updates;

    EicielParticipantListController* _controller;

    std::set<std::string> _users_list;
    std::set<std::string> _groups_list;

    ParticipantListActionMode _mode;

    void toggle_system_show();

    void change_participant_selection();
    void there_is_participant_selection();
    void there_is_participant_selection_default();
    void there_is_no_participant_selection();
    Glib::RefPtr<Gdk::Pixbuf> get_proper_icon(ElementKind e);
    void add_element(Glib::ustring title,
        bool reading,
        bool writing,
        bool execution,
        ElementKind e,
        Gtk::TreeModel::Row& row);
    void add_selected_participant(AddParticipantTarget target);
    void change_permissions(const Glib::ustring& str, PermissionKind p);

    void fill_participants(std::set<std::string>* participants,
        ElementKind kind,
        Glib::RefPtr<Gdk::Pixbuf> normal_icon,
        Glib::RefPtr<Gdk::Pixbuf> default_icon);

    void participants_list_double_click(
        AddParticipantTarget target,
        const Gtk::TreeModel::Path& p,
        Gtk::TreeViewColumn* c);

    void change_participant_kind();

    bool refilter();
    void on_clear_icon_pressed(Gtk::EntryIconPosition icon_position,
        const GdkEventButton* event);
    void filter_entry_text_changed();
    bool filter_participant_row(const Gtk::TreeModel::const_iterator&);

    void participant_entry_box_changed();
    void participant_entry_box_activate();

    bool enable_participant(const std::string& participant_name);

    void set_readonly(bool b);
    void can_edit_default_acl(bool b);

    void advanced_features_box_expanded();

    friend class EicielParticipantListController;
};

#endif
