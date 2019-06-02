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
#include "eiciel_main_window.hpp"

// Constructor
EicielACLWindow::EicielACLWindow(EicielACLWindowController* cont)
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
    /* GUI */
    , _info_bar()
    , _info_bar_vbox(Gtk::ORIENTATION_VERTICAL)
    , _info_bar_label("Applying ACLs to enclosed files")
    , _info_bar_progress()
    , _main_box(Gtk::ORIENTATION_VERTICAL)
    , _label_current_acl(_("<b>Current participants in ACL</b>"))
    , _acl_list(cont)
    , _label_participants(_("<b>Available participants</b>"))
    , _participant_list(cont)
    , _edit_enclosed_files(_("Edit ACLs for enclosed files..."))

    /* Non GUI */
    , _controller(cont) // Assign _controller to _window
{
    // Set the window to the controller
    _controller->_window = this;

    this->set_margin_top(12);
    this->set_margin_bottom(12);
    this->set_margin_start(12);
    this->set_margin_end(12);

    this->pack_start(_main_box);

    _label_current_acl.set_use_markup(true);
    _label_current_acl.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_START);
    _main_box.pack_start(_label_current_acl, Gtk::PACK_SHRINK);

    _main_box.pack_start(_acl_list);

    _label_participants.set_use_markup(true);
    _label_participants.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_START);
    _main_box.pack_start(_label_participants, Gtk::PACK_SHRINK);

    _main_box.pack_start(_participant_list);

    _edit_enclosed_files.set_margin_top(12);
    _edit_enclosed_files.set_halign(Gtk::ALIGN_START);
    _edit_enclosed_files.signal_clicked().connect(
            sigc::mem_fun(this, &EicielACLWindow::edit_enclosed_files));
    _main_box.pack_start(_edit_enclosed_files, Gtk::PACK_SHRINK);

    // Note we insert and remove _info_bar manually to work around
    // https://bugzilla.gnome.org/show_bug.cgi?id=710888
    _info_bar_label.set_halign(Gtk::ALIGN_START);
    _info_bar_vbox.pack_start(_info_bar_label, Gtk::PACK_SHRINK);

    _info_bar_progress.set_fraction(0.0);
    _info_bar_progress.set_hexpand(true);

    _info_bar_vbox.pack_start(_info_bar_progress, Gtk::PACK_SHRINK);

    _info_bar_vbox.set_spacing(12);
    Gtk::Container* info_bar_content_area = dynamic_cast<Gtk::Container*>(_info_bar.get_content_area());
    info_bar_content_area->add(_info_bar_vbox);

    show_all();
}

EicielACLWindow::~EicielACLWindow() {}

void EicielACLWindow::set_filename(const std::string& str)
{
    _main_box.set_sensitive(true);
}

void EicielACLWindow::set_active(bool b)
{
    _main_box.set_sensitive(b);
}

void EicielACLWindow::show_info_bar()
{
    this->pack_start(_info_bar);
    this->reorder_child(_info_bar, 0);
    _info_bar.show_all();
}

void EicielACLWindow::hide_info_bar()
{
    this->remove(_info_bar);
}

void EicielACLWindow::set_recursive_progress(double d)
{
    _info_bar_progress.set_fraction(d);
}

void EicielACLWindow::can_edit_enclosed_files(bool b)
{
    _edit_enclosed_files.set_sensitive(b);
}

void EicielACLWindow::edit_enclosed_files()
{
    _controller->edit_enclosed_files();
}

