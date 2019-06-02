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

#include "acl_element_kind.hpp"
#include "eiciel_enclosed_editor_window.hpp"
#include "eiciel_enclosed_editor_window_controller.hpp"
#include "eiciel_main_window_controller.hpp"
#include <giomm.h>
#include <thread>

EicielACLWindowController::EicielACLWindowController()
    : _ACL_manager(NULL)
    , _is_file_opened(false)
    , _last_error_message("")
{
}

EicielACLWindowController::~EicielACLWindowController()
{
    delete _ACL_manager;
}

void EicielACLWindowController::open_file(const std::string& s)
{
    try {
        ACLManager* new_manager;
        new_manager = new ACLManager(s);

        delete _ACL_manager;
        _ACL_manager = new_manager;

        redraw_acl_list();
        _window->set_filename(s);

        set_active(true);

        check_editable();

        _is_file_opened = true;
        _current_filename = s;
    } catch (ACLManagerException e) {
        _is_file_opened = false;
        _current_filename.clear();
        set_active(false);
        this->empty_acl_list();
        _last_error_message = e.getMessage();
    }
}

void EicielACLWindowController::update_acl_list()
{
    permissions_t effective_permissions(7);
    if (_ACL_manager->has_mask()) {
        effective_permissions = _ACL_manager->get_mask();
    }
    permissions_t effective_default_permissions(7);
    if (_ACL_manager->has_default_mask()) {
        effective_default_permissions = _ACL_manager->get_mask_default();
    }

    this->update_acl_ineffective(effective_permissions, effective_default_permissions);
}

void EicielACLWindowController::fill_acl_list(Glib::RefPtr<Gtk::ListStore> ref_acl_list, bool include_default_entries)
{
    permissions_t perms = _ACL_manager->get_user();
    std::vector<acl_entry> vACL;
    this->add_non_selectable(
        ref_acl_list,
        Glib::locale_to_utf8(_ACL_manager->get_owner_name()), perms.reading,
        perms.writing, perms.execution, EK_USER);

    vACL = _ACL_manager->get_acl_user();
    for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
         i++) {
        this->add_selectable(
            ref_acl_list,
            Glib::locale_to_utf8(i->name), i->reading, i->writing, i->execution,
            EK_ACL_USER);
    }

    perms = _ACL_manager->get_group();
    this->add_non_selectable(
        ref_acl_list,
        Glib::locale_to_utf8(_ACL_manager->get_group_name()), perms.reading,
        perms.writing, perms.execution, EK_GROUP);

    vACL = _ACL_manager->get_acl_group();
    for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
         i++) {
        this->add_selectable(
            ref_acl_list,
            Glib::locale_to_utf8(i->name), i->reading, i->writing, i->execution,
            EK_ACL_GROUP);
    }

    if (_ACL_manager->has_mask()) {
        perms = _ACL_manager->get_mask();
        this->add_non_selectable(
            ref_acl_list,
            _("Mask"), perms.reading, perms.writing,
            perms.execution, EK_MASK);
    }

    perms = _ACL_manager->get_other();
    this->add_non_selectable(
        ref_acl_list,
        _("Other"), perms.reading, perms.writing,
        perms.execution, EK_OTHERS);

    this->EicielACLListController::can_edit_default_acl(_ACL_manager->is_directory());
    can_edit_enclosed_files(_ACL_manager->is_directory());
    this->EicielParticipantListController::can_edit_default_acl(_ACL_manager->is_directory());
    this->default_acl_are_being_edited(false);

    if (include_default_entries && _ACL_manager->is_directory()) {
        bool there_is_default_acl = false;
        permissions_t effective_default_permissions(7);
        if (_ACL_manager->has_default_mask()) {
            effective_default_permissions = _ACL_manager->get_mask_default();
        }

        if (_ACL_manager->has_default_user()) {
            perms = _ACL_manager->get_user_default();
            this->add_non_selectable(
                ref_acl_list,
                Glib::locale_to_utf8(_ACL_manager->get_owner_name()), perms.reading,
                perms.writing, perms.execution, EK_DEFAULT_USER);
            there_is_default_acl = true;
        }

        vACL = _ACL_manager->get_acl_user_default();

        there_is_default_acl |= (vACL.size() > 0);
        for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
             i++) {
            this->add_selectable(
                ref_acl_list,
                Glib::locale_to_utf8(i->name), i->reading,
                i->writing, i->execution, EK_DEFAULT_ACL_USER);
        }

        if (_ACL_manager->has_default_group()) {
            perms = _ACL_manager->get_group_default();
            this->add_non_selectable(
                ref_acl_list,
                Glib::locale_to_utf8(_ACL_manager->get_group_name()), perms.reading,
                perms.writing, perms.execution, EK_DEFAULT_GROUP);
            there_is_default_acl |= true;
        }

        vACL = _ACL_manager->get_acl_group_default();

        there_is_default_acl |= (vACL.size() > 0);
        for (std::vector<acl_entry>::iterator i = vACL.begin(); i != vACL.end();
             i++) {
            this->add_selectable(
                ref_acl_list,
                Glib::locale_to_utf8(i->name), i->reading,
                i->writing, i->execution, EK_DEFAULT_ACL_GROUP);
        }

        if (_ACL_manager->has_default_mask()) {
            perms = _ACL_manager->get_mask_default();
            this->add_non_selectable(ref_acl_list, _("Default Mask"), perms.reading,
                perms.writing, perms.execution,
                EK_DEFAULT_MASK);
            there_is_default_acl |= true;
        }

        if (_ACL_manager->has_default_other()) {
            perms = _ACL_manager->get_other_default();
            this->add_non_selectable(ref_acl_list, _("Default Other"), perms.reading,
                perms.writing, perms.execution,
                EK_DEFAULT_OTHERS);
            there_is_default_acl |= true;
        }
        this->default_acl_are_being_edited(there_is_default_acl);
    }
}

void EicielACLWindowController::redraw_acl_list()
{
    Glib::RefPtr<Gtk::ListStore> ref_acl_list = this->create_acl_list_store();

    fill_acl_list(ref_acl_list, /* include_default_entries */ true);

    this->replace_acl_store(ref_acl_list);

    permissions_t effective_permissions(7);
    if (_ACL_manager->has_mask()) {
        effective_permissions = _ACL_manager->get_mask();
    }
    permissions_t effective_default_permissions(7);
    if (_ACL_manager->has_default_mask()) {
        effective_default_permissions = _ACL_manager->get_mask_default();
    }
    this->update_acl_ineffective(effective_permissions, effective_default_permissions);
}

bool EicielACLWindowController::is_directory()
{
    if (_ACL_manager == NULL)
        return false;

    return _ACL_manager->is_directory();
}

void EicielACLWindowController::add_acl_entry(AddParticipantTarget target,
        const std::string& s,
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

        this->choose_acl(s, e);
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

void EicielACLWindowController::remove_acl(const std::string& entry_name,
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

void EicielACLWindowController::update_acl_entry(ElementKind e,
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

typedef std::pair<std::string, bool> FileListItem;

static std::vector<FileListItem> get_all_files_recursively(Glib::RefPtr<Gio::File> directory)
{
    std::vector<FileListItem> result;
    if (!directory)
        return result;

    result.push_back(std::make_pair(directory->get_path(), true));

    Glib::RefPtr<Gio::FileEnumerator> file_enumerator = directory->enumerate_children(G_FILE_ATTRIBUTE_STANDARD_NAME, Gio::FILE_QUERY_INFO_NOFOLLOW_SYMLINKS);
    for (Glib::RefPtr<Gio::FileInfo> file_info = file_enumerator->next_file(); file_info; file_info = file_enumerator->next_file()) {
        bool is_dir = file_info->get_file_type() == Gio::FILE_TYPE_DIRECTORY;
        Glib::RefPtr<Gio::File> child = directory->get_child(file_info->get_name());
        if (!child)
            continue;
        if (is_dir) {
            std::vector<std::pair<std::string, bool>> result_children = get_all_files_recursively(child);
            result.insert(result.end(), result_children.begin(), result_children.end());
        } else {
            result.push_back(std::make_pair(child->get_path(), is_dir));
        }
    }
    return result;
}

void EicielACLWindowController::can_edit_enclosed_files(bool b)
{
    _window->can_edit_enclosed_files(b);
}

void EicielACLWindowController::edit_enclosed_files()
{
    EicielEnclosedEditorController enclosed_editor_controller;

    Gtk::Window* toplevel_window = (Gtk::Window*)_window->get_toplevel();
    EicielEnclosedEditor enclosed_editor(*toplevel_window, &enclosed_editor_controller);

    {
        EicielACLListController* acl_list_directory_controller = enclosed_editor_controller.get_acl_list_directory_controller();
        Glib::RefPtr<Gtk::ListStore> ref_acl_list_dirs = acl_list_directory_controller->create_acl_list_store();
        fill_acl_list(ref_acl_list_dirs, /* include_default_entries */ true);
        acl_list_directory_controller->replace_acl_store(ref_acl_list_dirs);

        permissions_t effective_permissions(7);
        if (_ACL_manager->has_mask()) {
            effective_permissions = _ACL_manager->get_mask();
        }
        permissions_t effective_default_permissions(7);
        if (_ACL_manager->has_default_mask()) {
            effective_default_permissions = _ACL_manager->get_mask_default();
        }
        acl_list_directory_controller->update_acl_ineffective(effective_permissions, effective_default_permissions);
        acl_list_directory_controller->can_edit_default_acl(true);
    }
    {
        EicielACLListController* acl_list_files_controller = enclosed_editor_controller.get_acl_list_file_controller();
        Glib::RefPtr<Gtk::ListStore> ref_acl_list_files = acl_list_files_controller->create_acl_list_store();
        fill_acl_list(ref_acl_list_files, /* include_default_entries */ false);
        acl_list_files_controller->replace_acl_store(ref_acl_list_files);

        permissions_t effective_permissions(7);
        if (_ACL_manager->has_mask()) {
            effective_permissions = _ACL_manager->get_mask();
        }
        permissions_t effective_default_permissions(7);
        if (_ACL_manager->has_default_mask()) {
            effective_default_permissions = _ACL_manager->get_mask_default();
        }
        acl_list_files_controller->update_acl_ineffective(effective_permissions, effective_default_permissions);
        acl_list_files_controller->can_edit_default_acl(false);
    }

    int response = enclosed_editor.run();
    if (response != Gtk::RESPONSE_APPLY)
        return;

    std::string directory_access_acl_text = enclosed_editor_controller.get_directory_access_acl_text();
    std::string directory_default_acl_text = enclosed_editor_controller.get_directory_default_acl_text();
    std::string file_access_acl_text = enclosed_editor_controller.get_file_access_acl_text();

    Glib::RefPtr<Gdk::Display> display = toplevel_window->get_display();
    Glib::RefPtr<Gdk::Cursor> clock_cursor = Gdk::Cursor::create(display, "wait");
    toplevel_window->get_window()->set_cursor(clock_cursor);
    set_active(false);

    _window->show_info_bar();

    std::thread apply_changes_thread(
        [this, toplevel_window, directory_access_acl_text, directory_default_acl_text, file_access_acl_text]() {
            // Now iterate all the files
            Glib::RefPtr<Gio::File> current_dir = Gio::File::create_for_path(_current_filename);
            if (!current_dir)
                return;

            // Recursively build list (including _current_filename)
            std::vector<FileListItem> all_filenames = get_all_files_recursively(current_dir);

            int current_file_num = 1, num_files = all_filenames.size();
            for (std::vector<FileListItem>::iterator it = all_filenames.begin();
                 it != all_filenames.end();
                 it++, current_file_num++) {
                std::string& filename = it->first;
                bool is_directory = it->second;
                try {
                    if (is_directory) {
                        ACLManager::set_file_acl(filename, directory_access_acl_text, directory_default_acl_text);
                    } else {
                        ACLManager::set_file_acl(filename, file_access_acl_text, "");
                    }
                } catch (ACLManagerException e) {
                    // FIXME: Can't do much here
                    std::cerr << "Exception when setting ACL of file '" << filename << "': " << e.getMessage() << "\n";
                } catch (...) {
                    // Catch-all to avoid crashing nautilus
                    ;
                }

                double percentage = (double)current_file_num / (double)num_files;
                Glib::signal_idle().connect_once([this, percentage]() {
                    _window->set_recursive_progress(percentage);
                });
            }
            Glib::signal_idle().connect_once([this, toplevel_window]() {
                toplevel_window->get_window()->set_cursor();
                // Update file
                open_file(_current_filename);
                _window->hide_info_bar();
            });
        });
    apply_changes_thread.detach();
}

// Returns true if the changed happened
bool EicielACLWindowController::toggle_edit_default_acl(bool default_acl_were_being_edited)
{
    try {
        if (default_acl_were_being_edited) {
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
            if (result == Gtk::RESPONSE_NO)
                return false;
            _ACL_manager->clear_default_acl();
        } else {
            _ACL_manager->create_default_acl();
        }
        redraw_acl_list();
        return true;
    } catch (ACLManagerException e) {
        _last_error_message = e.getMessage();
        return false;
    }
}

bool EicielACLWindowController::opened_file()
{
    return _is_file_opened;
}

void EicielACLWindowController::set_readonly(bool b)
{
    this->EicielACLListController::set_readonly(b);
    this->EicielParticipantListController::set_readonly(b);
}

void EicielACLWindowController::check_editable()
{
    /*
   * In Linux we should check CAP_FOWNER capability. At the moment give a
   * rough approach where whe check whether the user is the owner or root.
   *
   * I've not looked for what should be checked in FreeBSD
   */
    uid_t real_user = getuid();
    if ((real_user != 0) && (real_user != _ACL_manager->get_owner_uid())) {
        this->set_readonly(true);
    } else {
        this->set_readonly(false);
    }
}

Glib::ustring EicielACLWindowController::last_error()
{
    return _last_error_message;
}

void EicielACLWindowController::set_active(bool b)
{
    EicielACLListController::set_active(b);
    EicielParticipantListController::set_active(b);
    _window->set_active(b);
}
