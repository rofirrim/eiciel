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

#include "eiciel_enclosed_editor_window.hpp"
#include "eiciel_enclosed_editor_window_controller.hpp"

EicielEnclosedEditor::EicielEnclosedEditor(Gtk::Window& parent, EicielEnclosedEditorController* cont)
    : Gtk::Dialog(_("Edit ACLs for enclosed files"), parent, Gtk::DIALOG_MODAL | Gtk::DIALOG_USE_HEADER_BAR)
    , _cancel_changes(_("_Cancel"), /* mnemonic */ true)
    , _apply_changes(_("_Apply"), /* mnemonic */ true)
    , _main_box(Gtk::ORIENTATION_VERTICAL)
    , _acl_editors_hbox(Gtk::ORIENTATION_HORIZONTAL)
    , _left_box(Gtk::ORIENTATION_VERTICAL)
    , _label_acl_directories(_("<b>Participants in ACL for enclosed directories</b>"))
    , _acl_list_directories(cont->get_acl_list_directory_controller())
    , _right_box(Gtk::ORIENTATION_VERTICAL)
    , _label_acl_files(_("<b>Participants in ACL for enclosed files</b>"))
    , _acl_list_files(cont->get_acl_list_file_controller())
    , _participants_box(Gtk::ORIENTATION_VERTICAL)
    , _label_participants(_("<b>Available participants</b>"))
    , _participant_list(cont->get_participant_list_controller())
    , _controller(cont)
{
    _controller->_window = this;
    
    set_deletable(false);

    get_vbox()->set_margin_top(12);
    get_vbox()->set_margin_bottom(12);
    get_vbox()->set_margin_start(12);
    get_vbox()->set_margin_end(12);

    get_vbox()->pack_start(_main_box);
    _main_box.pack_start(_acl_editors_hbox);

    // ACL editors
    _label_acl_directories.set_use_markup(true);
    _label_acl_directories.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_START);

    _acl_editors_hbox.pack_start(_left_box);
    _left_box.pack_start(_label_acl_directories, Gtk::PACK_SHRINK);
    _left_box.pack_start(_acl_list_directories);

    _label_acl_files.set_use_markup(true);
    _label_acl_files.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_START);

    _acl_editors_hbox.pack_start(_right_box);
    _right_box.pack_start(_label_acl_files, Gtk::PACK_SHRINK);
    _right_box.pack_start(_acl_list_files);
    _acl_list_files.disable_default_acl_editing();

    // Participants box
    _main_box.pack_start(_participants_box);

    _label_participants.set_use_markup(true);
    _label_participants.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_START);

    _participants_box.pack_start(_label_participants, Gtk::PACK_SHRINK);
    _participants_box.pack_start(_participant_list);
    _participant_list.set_mode(EicielParticipantList::ENCLOSED_FILES_EDITOR);

    // Headerbar
    Gtk::HeaderBar *hbar = get_header_bar();
    hbar->pack_start(_cancel_changes);
    _cancel_changes.signal_clicked().connect(
        sigc::bind<0>(sigc::mem_fun(this, &Gtk::Dialog::response), Gtk::RESPONSE_CANCEL));
    hbar->pack_end(_apply_changes);
    _apply_changes.signal_clicked().connect(
            sigc::mem_fun(this, &EicielEnclosedEditor::apply_changes));

    show_all();
}

EicielEnclosedEditor::~EicielEnclosedEditor() {}

void EicielEnclosedEditor::apply_changes()
{
    _controller->apply_changes();
}
