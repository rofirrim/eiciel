/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2004-2022 Roger Ferrer Ibáñez

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
   USA
*/
#include "config.h"
#include "eiciel/acl_editor_controller.h"
#include "eiciel/acl_element_kind.h"
#include "eiciel/enclosed_acl_editor_widget.h"
#include "eiciel/enclosed_acl_editor_controller.h"
#include <giomm.h>
#include <gdkmm/cursor.h>
#include <thread>

namespace eiciel {

ACLEditorController::ACLEditorController()
    : acl_manager(nullptr), is_file_opened(false) {}

ACLEditorController::~ACLEditorController() { delete acl_manager; }

void ACLEditorController::open_file(const Glib::ustring &s) {
  try {
    ACLManager *new_manager;
    new_manager = new ACLManager(s);

    delete acl_manager;
    acl_manager = new_manager;

    redraw_acl_list();

    set_active(true);

    check_editable();

    is_file_opened = true;
    current_filename = s;
  } catch (ACLManagerException e) {
    is_file_opened = false;
    current_filename.clear();
    set_active(false);
    this->empty_acl_list();
  }
}

void ACLEditorController::update_acl_list() {
  permissions_t effective_permissions(7);
  if (acl_manager->has_mask()) {
    effective_permissions = acl_manager->get_mask();
  }
  permissions_t effective_default_permissions(7);
  if (acl_manager->has_default_mask()) {
    effective_default_permissions = acl_manager->get_mask_default();
  }

  this->update_acl_ineffective(effective_permissions,
                               effective_default_permissions);
}

void ACLEditorController::redraw_acl_list() {
  // FIXME???
  create_acl_list_store();

  fill_acl_list(/* include_default_entries */ true);
  
  replace_acl_store();

  permissions_t effective_permissions(7);
  if (acl_manager->has_mask()) {
    effective_permissions = acl_manager->get_mask();
  }
  permissions_t effective_default_permissions(7);
  if (acl_manager->has_default_mask()) {
    effective_default_permissions = acl_manager->get_mask_default();
  }
  this->update_acl_ineffective(effective_permissions,
                               effective_default_permissions);
}

void ACLEditorController::fill_acl_list(bool include_default_entries) {
  ACLListController::fill_acl_list(*acl_manager, include_default_entries);
  ParticipantListController::can_edit_default_acl(acl_manager->is_directory());
  view->can_edit_enclosed_files(acl_manager->is_directory());
}

bool ACLEditorController::is_directory() {
  if (acl_manager == NULL)
    return false;

  return acl_manager->is_directory();
}

void ACLEditorController::add_acl_entry(AddParticipantTarget target,
                                        const Glib::ustring &s, ElementKind e,
                                        bool is_default) {
  permissions_t p(7);

  if (is_default) {
    if (e == ElementKind::acl_user) {
      e = ElementKind::default_acl_user;
    } else if (e == ElementKind::acl_group) {
      e = ElementKind::default_acl_group;
    }
  }

  try {
    switch (e) {
    case ElementKind::acl_user: {
      acl_manager->modify_acl_user(s, p);
      break;
    }
    case ElementKind::acl_group: {
      acl_manager->modify_acl_group(s, p);
      break;
    }
    case ElementKind::default_acl_user: {
      acl_manager->modify_acl_default_user(s, p);
      break;
    }
    case ElementKind::default_acl_group: {
      acl_manager->modify_acl_default_group(s, p);
      break;
    }
    default:
      break;
    }

    redraw_acl_list();
    highlight_acl_entry(s, e);
  } catch (ACLManagerException e) {
    Glib::ustring s = _("Could not add ACL entry: ") + e.getMessage();
    show_message_dialog_error(s);
  }
}

void ACLEditorController::show_message_dialog_error(const Glib::ustring &s) {
  Gtk::MessageDialog *message_dialog;
  Gtk::Window *w = nullptr;
  if (this->view && (w = dynamic_cast<Gtk::Window*>(this->view->get_root()))) {
    message_dialog =
        new Gtk::MessageDialog(*w, s, false, Gtk::MessageType::ERROR,
                               Gtk::ButtonsType::OK, /* modal */ true);
  } else {
    message_dialog =
        new Gtk::MessageDialog(s, false, Gtk::MessageType::ERROR,
                               Gtk::ButtonsType::OK, /* modal */ true);
  }
  message_dialog->signal_response().connect([message_dialog](int) mutable {
    delete message_dialog;
    message_dialog = nullptr;
  });
  message_dialog->show();
}

bool ACLEditorController::remove_acl(const Glib::ustring &entry_name,
                                     ElementKind e) {
  bool updated = true;
  try {
    switch (e) {
    case ElementKind::acl_user: {
      acl_manager->remove_acl_user(entry_name);
      break;
    }
    case ElementKind::acl_group: {
      acl_manager->remove_acl_group(entry_name);
      break;
    }
    case ElementKind::default_acl_user: {
      acl_manager->remove_acl_user_default(entry_name);
      break;
    }
    case ElementKind::default_acl_group: {
      acl_manager->remove_acl_group_default(entry_name);
      break;
    }
    default:
      updated = false;
      break;
    }
  } catch (ACLManagerException e) {
    Glib::ustring s = _("Could not remove ACL entry: ") + e.getMessage();
    show_message_dialog_error(s);
    updated = false;
  }
  return updated;
}

void ACLEditorController::update_acl_entry(ElementKind e,
                                           const Glib::ustring &s, bool reading,
                                           bool writing, bool execution) {
  permissions_t p(reading, writing, execution);

  try {
    switch (e) {
    case ElementKind::mask: {
      acl_manager->modify_mask(p);
      break;
    }
    case ElementKind::default_mask: {
      acl_manager->modify_mask_default(p);
      break;
    }
    case ElementKind::user: {
      acl_manager->modify_owner_perms(p);
      break;
    }
    case ElementKind::group: {
      acl_manager->modify_group_perms(p);
      break;
    }
    case ElementKind::others: {
      acl_manager->modify_others_perms(p);
      break;
    }
    case ElementKind::default_user: {
      acl_manager->modify_owner_perms_default(p);
      break;
    }
    case ElementKind::default_group: {
      acl_manager->modify_group_perms_default(p);
      break;
    }
    case ElementKind::default_others: {
      acl_manager->modify_others_perms_default(p);
      break;
    }
    case ElementKind::acl_user: {
      acl_manager->modify_acl_user(s, p);
      break;
    }
    case ElementKind::acl_group: {
      acl_manager->modify_acl_group(s, p);
      break;
    }
    case ElementKind::default_acl_user: {
      acl_manager->modify_acl_default_user(s, p);
      break;
    }
    case ElementKind::default_acl_group: {
      acl_manager->modify_acl_default_group(s, p);
      break;
    }
    default:
      break;
    }
    update_acl_list();
  } catch (ACLManagerException e) {
    Glib::ustring s = _("Could not modify ACL entry: ") + e.getMessage();
    show_message_dialog_error(s);
  }
}

typedef std::pair<Glib::ustring, bool> FileListItem;

static std::vector<FileListItem>
get_all_files_recursively(Glib::RefPtr<Gio::File> directory) {
  std::vector<FileListItem> result;
  if (!directory)
    return result;

  result.push_back(std::make_pair(directory->get_path(), true));

  Glib::RefPtr<Gio::FileEnumerator> file_enumerator =
      directory->enumerate_children(G_FILE_ATTRIBUTE_STANDARD_NAME,
                                    Gio::FileQueryInfoFlags::NOFOLLOW_SYMLINKS);
  for (Glib::RefPtr<Gio::FileInfo> file_info = file_enumerator->next_file();
       file_info; file_info = file_enumerator->next_file()) {
    bool is_dir = file_info->get_file_type() == Gio::FileType::DIRECTORY;
    Glib::RefPtr<Gio::File> child = directory->get_child(file_info->get_name());
    if (!child)
      continue;
    if (is_dir) {
      std::vector<std::pair<Glib::ustring, bool>> result_children =
          get_all_files_recursively(child);
      result.insert(result.end(), result_children.begin(),
                    result_children.end());
    } else {
      result.push_back(std::make_pair(child->get_path(), is_dir));
    }
  }
  return result;
}

void ACLEditorController::can_edit_enclosed_files(bool b) {
  view->can_edit_enclosed_files(b);
}

void ACLEditorController::apply_recursively(
    EnclosedACLEditorController *enclosed_editor_controller,
    Gtk::Window *toplevel_window) {
  g_return_if_fail(enclosed_editor_controller);

  enclosed_editor_controller->compute_acl_textual_representation();
  Glib::ustring directory_access_acl_text =
      enclosed_editor_controller->get_directory_access_acl_text();
  Glib::ustring directory_default_acl_text =
      enclosed_editor_controller->get_directory_default_acl_text();
  Glib::ustring file_access_acl_text =
      enclosed_editor_controller->get_file_access_acl_text();

  Glib::RefPtr<Gdk::Display> display = toplevel_window->get_display();
  Glib::RefPtr<Gdk::Cursor> clock_cursor = Gdk::Cursor::create("wait");
  toplevel_window->set_cursor(clock_cursor);
  set_active(false);

  view->show_info_bar();

  std::thread apply_changes_thread(
      [this, toplevel_window, directory_access_acl_text,
       directory_default_acl_text, file_access_acl_text,
       enclosed_editor_controller]() {
        // Now iterate all the files
        Glib::RefPtr<Gio::File> current_dir =
            Gio::File::create_for_path(current_filename);
        if (!current_dir)
          return;

        // Recursively build list (including current_filename)
        std::vector<FileListItem> all_filenames =
            get_all_files_recursively(current_dir);

        int current_file_num = 1, num_files = all_filenames.size();
        for (std::vector<FileListItem>::iterator it = all_filenames.begin();
             it != all_filenames.end(); it++, current_file_num++) {
          Glib::ustring &filename = it->first;
          bool is_directory = it->second;
          try {
            if (is_directory) {
              ACLManager::set_file_acl(filename, directory_access_acl_text,
                                       directory_default_acl_text);
            } else {
              ACLManager::set_file_acl(filename, file_access_acl_text, "");
            }
          } catch (ACLManagerException e) {
            // FIXME: Can't do much here
            g_warning(_("Exception when setting ACL of file '%s': '%s'"),
                      filename.c_str(), e.getMessage().c_str());
          } catch (...) {
            // Catch-all to avoid crashing nautilus
            g_warning(_("Unknown exception when setting ACL of file '%s'"),
                      filename.c_str());
          }

          double percentage = (double)current_file_num / (double)num_files;
          Glib::signal_idle().connect_once([this, percentage]() {
            view->set_recursive_progress(percentage);
          });
        }
        Glib::signal_idle().connect_once(
            [this, toplevel_window, enclosed_editor_controller]() {
              toplevel_window->set_cursor();
              // Update file
              open_file(current_filename);
              view->hide_info_bar();
              delete enclosed_editor_controller;
            });
      });
  apply_changes_thread.detach();
}

void ACLEditorController::edit_enclosed_files() {
  EnclosedACLEditorController *enclosed_editor_controller =
      new EnclosedACLEditorController();

  Gtk::Window *toplevel_window =
      dynamic_cast<Gtk::Window *>(this->view->get_root());
  g_return_if_fail(toplevel_window);

  EnclosedACLEditorWidget *enclosed_editor =
      EnclosedACLEditorWidget::create(enclosed_editor_controller);
  enclosed_editor->set_transient_for(*toplevel_window);
  enclosed_editor->set_modal(true);

  enclosed_editor->signal_response().connect(
      [this, enclosed_editor, enclosed_editor_controller,
       toplevel_window](int response) mutable {
        g_return_if_fail(enclosed_editor);
        if (response == Gtk::ResponseType::APPLY) {
          // Transferring ownership here.
          apply_recursively(enclosed_editor_controller, toplevel_window);
        } else {
          delete enclosed_editor_controller;
        }
        delete enclosed_editor;
        enclosed_editor = nullptr;
      });

  {
    ACLListController *acl_list_files_controller =
        enclosed_editor_controller->get_acl_list_file_controller();
    acl_list_files_controller->set_view(
        enclosed_editor->get_files_acl_list_widget());
    acl_list_files_controller->fill_acl_list(
        *acl_manager, /* include_default_entries */ false);
    acl_list_files_controller->replace_acl_store();

    permissions_t effective_permissions(7);
    if (acl_manager->has_mask()) {
      effective_permissions = acl_manager->get_mask();
    }
    permissions_t effective_default_permissions(7);
    if (acl_manager->has_default_mask()) {
      effective_default_permissions = acl_manager->get_mask_default();
    }
    acl_list_files_controller->update_acl_ineffective(
        effective_permissions, effective_default_permissions);
    acl_list_files_controller->can_edit_default_acl(false);
  }

  {
    ACLListController *acl_list_directory_controller =
        enclosed_editor_controller->get_acl_list_directory_controller();
    acl_list_directory_controller->set_view(
        enclosed_editor->get_dirs_acl_list_widget());

    acl_list_directory_controller->create_acl_list_store();
    acl_list_directory_controller->replace_acl_store();
    acl_list_directory_controller->fill_acl_list(
        *acl_manager, /* include_default_entries */ true);

    permissions_t effective_permissions(7);
    if (acl_manager->has_mask()) {
      effective_permissions = acl_manager->get_mask();
    }
    permissions_t effective_default_permissions(7);
    if (acl_manager->has_default_mask()) {
      effective_default_permissions = acl_manager->get_mask_default();
    }
    acl_list_directory_controller->update_acl_ineffective(
        effective_permissions, effective_default_permissions);
    acl_list_directory_controller->can_edit_default_acl(true);
  }

  enclosed_editor->show();
}

// Returns true if the changed happened
void ACLEditorController::toggle_edit_default_acl(
    bool default_acl_were_being_edited,
    std::function<void(bool)> callback) {
  try {
    if (default_acl_were_being_edited) {
      Glib::ustring s(
          _("Are you sure you want to remove all ACL default entries?"));
      Gtk::Window *toplevel =
          dynamic_cast<Gtk::Window *>(this->view->get_root());
      Gtk::MessageDialog *remove_acl_message;
      if (toplevel == NULL) {
        remove_acl_message = new Gtk::MessageDialog(
            s, false, Gtk::MessageType::QUESTION, Gtk::ButtonsType::YES_NO);
      } else {
        remove_acl_message = new Gtk::MessageDialog(*toplevel, s, false,
                                                    Gtk::MessageType::QUESTION,
                                                    Gtk::ButtonsType::YES_NO);
      }
      remove_acl_message->set_modal(true);
      remove_acl_message->signal_response().connect(
          [this, remove_acl_message, callback](int response) mutable {
            if (response == Gtk::ResponseType::YES) {
              acl_manager->clear_default_acl();
              redraw_acl_list();
            }
            callback(response == Gtk::ResponseType::YES);
            delete remove_acl_message;
            remove_acl_message = nullptr;
          });
      remove_acl_message->show();
    } else {
      acl_manager->create_default_acl();
      redraw_acl_list();
    }
  } catch (ACLManagerException e) {
  }
}

bool ACLEditorController::opened_file() { return is_file_opened; }

void ACLEditorController::set_readonly(bool b) {
  this->ACLListController::set_readonly(b);
  this->ParticipantListController::set_readonly(b);
}

void ACLEditorController::check_editable() {
  /*
   * In Linux we should check CAP_FOWNER capability. At the moment give a
   * rough approach where whe check whether the user is the owner or root.
   *
   * I've not looked for what should be checked in FreeBSD
   */
  uid_t real_user = getuid();
  if ((real_user != 0) && (real_user != acl_manager->get_owner_uid())) {
    this->set_readonly(true);
  } else {
    this->set_readonly(false);
  }
}

void ACLEditorController::set_active(bool b) {
  ACLListController::set_active(b);
  ParticipantListController::set_active(b);
  view->set_active(b);
}

} // namespace eiciel
