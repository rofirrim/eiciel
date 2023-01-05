/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2022 Roger Ferrer Ibáñez

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
#include "eiciel/acl_list_widget.h"
#include <cassert>
#include <gtkmm/builder.h>
#include <gtkmm/listview.h>
#include <gtkmm/columnview.h>
#include <gtkmm/columnviewcolumn.h>
#include <gtkmm/expression.h>
#include <gtkmm/label.h>
#include <gtkmm/listitem.h>
#include <gtkmm/noselection.h>
#include <gtkmm/signallistitemfactory.h>
#include <glibmm/variant.h>
#include <iostream>
#include <memory>

namespace eiciel {

GType ACLListWidget::gtype = 0;

ACLListWidget::ACLListWidget(ACLListController *cont,
                             ACLListWidgetMode widget_mode)
    : Glib::ObjectBase("ACLListWidget"), controller(cont),
      readonly_mode(*this, "readonly-mode", false),
      exist_ineffective_permissions(*this, "exist-ineffective-permissions",
                                    false),
      toggling_default_acl(false), widget_mode(widget_mode) {
  controller->set_view(this);

  // Create UI from Resource
  auto refBuilder = Gtk::Builder::create_from_resource(
      "/org/roger_ferrer/eiciel/acl_list_widget.ui");

  auto main_box = refBuilder->get_widget<Gtk::Box>("top-level");
  append(*main_box);

  column_view = refBuilder->get_widget<Gtk::ColumnView>("column-view");
  edit_default_participants =
      refBuilder->get_widget<Gtk::ToggleButton>("edit-default-participants");
  edit_default_participants->signal_toggled().connect(
      [this]() { toggle_edit_default_acl(); });

  warning_icon = refBuilder->get_widget<Gtk::Image>("warning-icon");
  warning_label = refBuilder->get_widget<Gtk::Label>("warning-label");

  // Model
  model = ACLListItemModel::create();
  column_view->set_model(Gtk::NoSelection::create(model));

  Glib::RefPtr<Gtk::ColumnViewColumn> column;

  auto factory_icon = Gtk::SignalListItemFactory::create();
  factory_icon->signal_setup().connect(
      [&](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto image = Gtk::make_managed<Gtk::Image>();
        li->set_child(*image);
        image->set_pixel_size(24);
        auto label_expr =
            Gtk::PropertyExpression<Glib::RefPtr<Glib::ObjectBase>>::create(
                Gtk::ListItem::get_type(), "item");
        auto icon_name_expr = Gtk::PropertyExpression<Glib::ustring>::create(
            ACLItem::get_type(), label_expr, "resource-path");
        icon_name_expr->bind(image->property_resource(), li);
      });

  column = Gtk::ColumnViewColumn::create("", factory_icon);
  column_view->append_column(column);

  this->readonly_mode.get_proxy().signal_changed().connect([this]() {
   edit_default_participants->set_sensitive(!this->readonly_mode.get_value());
  });

  auto factory_name = Gtk::SignalListItemFactory::create();
  factory_name->signal_setup().connect(
      [&](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto label = Gtk::make_managed<Gtk::Label>();
        label->set_xalign(0);
        li->set_child(*label);

        // This is black magic that basically binds li->"item"->"name" with
        // label->"label".
        auto label_expr =
            Gtk::PropertyExpression<Glib::RefPtr<Glib::ObjectBase>>::create(
                Gtk::ListItem::get_type(), "item");
        auto name_expr = Gtk::PropertyExpression<Glib::ustring>::create(
            ACLItem::get_type(), label_expr, "name");
        name_expr->bind(label->property_label(), li);
      });

  column = Gtk::ColumnViewColumn::create(_("Participant"), factory_name);
  column_view->append_column(column);

  auto factory_permission_maker = [this](Glib::ustring permission,
                                         Glib::ustring masked_permission) {
    auto factory_permission = Gtk::SignalListItemFactory::create();
    factory_permission->signal_setup().connect(
        [this, permission,
         masked_permission](const Glib::RefPtr<Gtk::ListItem> &li) {
          auto box =
              Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
          li->set_child(*box);
          auto cb = Gtk::make_managed<Gtk::CheckButton>();
          box->append(*cb);
          auto image = Gtk::make_managed<Gtk::Image>();
          box->append(*image);

          auto label_expr =
              Gtk::PropertyExpression<Glib::RefPtr<Glib::ObjectBase>>::create(
                  Gtk::ListItem::get_type(), "item");

          Glib::Binding::bind_property(
              this->readonly_mode.get_proxy(), cb->property_sensitive(),
              Glib::Binding::Flags::DEFAULT, [](bool b) { return !b; });

          // Bind li->"item"->${permission} with cb->"active"
          auto permission_expr = Gtk::PropertyExpression<bool>::create(
              ACLItem::get_type(), label_expr, permission);
          permission_expr->bind(cb->property_active(), li);

          // Bind li->"item"->${masked_permission} with image->"visible"
          image->set_from_icon_name("dialog-warning-symbolic");
          auto ineffective_permission_expr =
              Gtk::PropertyExpression<bool>::create(
                  ACLItem::get_type(), label_expr, masked_permission);
          ineffective_permission_expr->bind(image->property_visible(), li);
        });
    factory_permission->signal_bind().connect(
        [this, permission, c = controller](Glib::RefPtr<Gtk::ListItem> li) {
          auto box = dynamic_cast<Gtk::Box *>(li->get_child());
          auto checkbutton =
              dynamic_cast<Gtk::CheckButton *>(box->get_first_child());
          auto image =
              dynamic_cast<Gtk::Image *>(checkbutton->get_next_sibling());
          if (!checkbutton || !image)
            return;
          remove_checkbutton_signal(checkbutton);
          auto item = li->get_item();
          if (auto acl_item = std::dynamic_pointer_cast<ACLItem>(item)) {
            auto sig = checkbutton->signal_toggled().connect(
                [this, c, permission, acl_item, checkbutton]() {
                  change_permissions(acl_item, checkbutton, permission);
                });
            keep_checkbutton_signal(checkbutton, sig);
          }
        });
    factory_permission->signal_unbind().connect(
        [this](const Glib::RefPtr<Gtk::ListItem> &li) {
          auto box = dynamic_cast<Gtk::Box *>(li->get_child());
          auto checkbutton =
              dynamic_cast<Gtk::CheckButton *>(box->get_first_child());
          if (!checkbutton)
            return;
          remove_checkbutton_signal(checkbutton);
        });
    return factory_permission;
  };

  auto factory_r = factory_permission_maker("r", "masked_r");
  column = Gtk::ColumnViewColumn::create(_("Read"), factory_r);
  column_view->append_column(column);
  auto factory_w = factory_permission_maker("w", "masked_w");
  column = Gtk::ColumnViewColumn::create(_("Write"), factory_w);
  column_view->append_column(column);
  auto factory_x = factory_permission_maker("x", "masked_x");
  column = Gtk::ColumnViewColumn::create(_("Execute"), factory_x);
  column_view->append_column(column);

  auto factory_remove_button = Gtk::SignalListItemFactory::create();
  factory_remove_button->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &li) {
        li->set_child(*Gtk::make_managed<Gtk::Button>());
      });
  factory_remove_button->signal_bind().connect(
      [this, c = controller](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto button = dynamic_cast<Gtk::Button *>(li->get_child());
        if (!button)
          return;
        remove_button_signal(button);
        auto item = li->get_item();
        button->set_icon_name("edit-delete-symbolic");
        if (auto participant_item = std::dynamic_pointer_cast<ACLItem>(item)) {
          if (participant_item->can_be_removed()) {
            button->set_visible(true);
            auto sig = button->signal_clicked().connect(
                [this, participant_item, c, li]() {
                  bool result = c->remove_acl(participant_item->get_name(),
                                              participant_item->get_kind());
                  if (result) {
                    model->remove(li->get_position());
                    remove_unneeded_entries();
                  }
                });
            keep_button_signal(button, sig);
          } else {
            button->set_visible(false);
          }
        }
      });
  factory_remove_button->signal_unbind().connect(
      [this](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto button = dynamic_cast<Gtk::Button *>(li->get_child());
        if (!button)
          return;
        remove_button_signal(button);
      });
  column = Gtk::ColumnViewColumn::create("", factory_remove_button);
  column_view->append_column(column);

  if (widget_mode == ACLListWidgetMode::ONLY_DIRECTORY) {
    can_edit_default_acl(true);
  } else if (widget_mode == ACLListWidgetMode::ONLY_FILE) {
    can_edit_default_acl(true);
    edit_default_participants->set_visible(false);
  }

  exist_ineffective_permissions.get_proxy().signal_changed().connect([this]() {
    bool b = exist_ineffective_permissions.get_value();
    warning_label->set_visible(b);
    warning_icon->set_visible(b);
  });
}

ACLListWidget::~ACLListWidget() {}

void ACLListWidget::set_active(bool b) { set_sensitive(b); }

void ACLListWidget::set_readonly(bool b) {
  readonly_mode.set_value(b);
}

void ACLListWidget::highlight_acl_entry(const Glib::ustring &name,
                                        ElementKind kind) {
  // Check first if was already inserted
  guint idx, end = model->get_n_items();
  bool found = false;
  for (idx = 0; idx < end; idx++) {
    auto elem = model->get_item(idx);
    if (elem->get_kind() == kind && elem->get_name() == name) {
      found = true;
      break;
    }
  }

  if (!found)
    return;

  // Adapted from nautilus.
  Gtk::Widget *child = column_view->get_last_child();
  while (child != NULL && !dynamic_cast<Gtk::ListView *>(child)) {
    child = child->get_prev_sibling();
  }

  if (child != NULL) {
    child->activate_action("list.scroll-to-item",
                           Glib::Variant<guint>::create(idx));
  }
}

// FIXME???
void ACLListWidget::create_acl_list_store() { empty_acl_list(); }
void ACLListWidget::empty_acl_list() {
  model->remove_all();
}
void ACLListWidget::replace_acl_store() { }

void ACLListWidget::add_non_removable(Glib::ustring title, bool reading,
                                       bool writing, bool execution,
                                       ElementKind e) {
  auto new_item = ACLItem::create(e, title, reading, writing, execution,
                                  /* removable */ false);
  model->append(new_item);
}

void ACLListWidget::add_removable(Glib::ustring title, bool reading,
                                   bool writing, bool execution,
                                   ElementKind e) {
  auto new_item = ACLItem::create(e, title, reading, writing, execution,
                                  /* removable */ true);
  model->append(new_item);
}

void ACLListWidget::add_element(Glib::ustring title, bool reading, bool writing,
                                bool execution, ElementKind e) {
  auto new_item = ACLItem::create(e, title, reading, writing, execution,
                                  /* removable */ true);
}

void ACLListWidget::insert_before(const Glib::ustring &s,
                                  ElementKind new_element_kind,
                                  ElementKind before_element_kind,
                                  bool removable) {
  // Check first if was already inserted and also compute where it should go.
  guint idx, end = model->get_n_items();
  std::optional<guint> insert_idx;
  for (idx = 0; idx < end; idx++) {
    auto elem = model->get_item(idx);
    if (elem->get_kind() == new_element_kind && elem->get_name() == s)
      return;
    if (!insert_idx && elem->get_kind() == before_element_kind) {
      insert_idx = idx;
    }
  }

  if (!insert_idx)
    return;

  auto new_item =
      ACLItem::create(new_element_kind, s, /* r */ true, /* w */ true,
                      /* x */ true, /* removable */ removable);
  model->insert(*insert_idx, new_item);
}

void ACLListWidget::populate_required_nondefault_entries() {
  insert_before(_("Mask"), ElementKind::mask, ElementKind::others, /* removable */ false);
}

void ACLListWidget::populate_required_default_entries() {
  guint idx, nitems = model->get_n_items();
  Glib::ustring user_owner, group_owner;
  for (idx = 0; idx < nitems; idx++) {
    auto elem = model->get_item(idx);
    switch (elem->get_kind()) {
    default:
      break;
    case ElementKind::user:
      user_owner = Glib::ustring(elem->get_name());
      break;
    case ElementKind::group:
      group_owner = Glib::ustring(elem->get_name());
      break;
    case ElementKind::default_others:
      // We assume that an ElementKind::default_others means we are including
      // default entries so give up if already found.
      return;
    }
  }

  // ElementKind::default_others must be the last, so we handle it here manually
  add_non_removable(_("Default Other"),
                     /* reading */ true,
                     /* writing */ true,
                     /* execution */ true, ElementKind::default_others);

  insert_before(_("Default Mask"), ElementKind::default_mask,
                ElementKind::default_others,
                /* removable */ false);

  insert_before(group_owner, ElementKind::default_group,
                ElementKind::default_mask,
                /* removable */ false);
  insert_before(user_owner, ElementKind::default_user,
                ElementKind::default_group,
                /* removable */ false);

  default_acl_are_being_edited(true);
}

void ACLListWidget::remove_all_default_entries() {
  // The ACL is empty, remove irrelevant entries now
  guint idx, nitems = model->get_n_items();
  for (idx = 0; idx < nitems;) {
    auto elem = model->get_item(idx);
    switch (elem->get_kind()) {
    case ElementKind::default_acl_user:
    case ElementKind::default_acl_group:
    case ElementKind::default_user:
    case ElementKind::default_group:
    case ElementKind::default_mask:
    case ElementKind::default_others:
      model->remove(idx);
      nitems = model->get_n_items();
      break;
    default:
      idx++;
    }
  }
}

void ACLListWidget::insert_user(const Glib::ustring &s) {
  populate_required_nondefault_entries();
  insert_before(s, ElementKind::acl_user, ElementKind::group,
                /* removable */ true);
}

void ACLListWidget::insert_group(const Glib::ustring &s) {
  populate_required_nondefault_entries();
  insert_before(s, ElementKind::acl_group, ElementKind::mask,
                /* removable */ true);
}

void ACLListWidget::insert_default_user(const Glib::ustring &s) {
  populate_required_default_entries();
  insert_before(s, ElementKind::default_acl_user, ElementKind::default_group,
                /* removable */ true);
}

void ACLListWidget::insert_default_group(const Glib::ustring &s) {
  populate_required_default_entries();
  insert_before(s, ElementKind::default_acl_group, ElementKind::default_mask,
                /* removable */ true);
}

bool ACLListWidget::nondefault_acl_is_empty() {
  guint idx, nitems = model->get_n_items();
  for (idx = 0; idx < nitems; idx++) {
    auto elem = model->get_item(idx);
    switch (elem->get_kind()) {
    case ElementKind::acl_user:
    case ElementKind::acl_group:
      return false;
    default:
      break;
    }
  }
  return true;
}

void ACLListWidget::remove_unneeded_entries() {
  if (nondefault_acl_is_empty()) {
    // The ACL is empty, remove irrelevant entries now
    guint idx, nitems = model->get_n_items();
    for (idx = 0; idx < nitems;) {
      auto elem = model->get_item(idx);
      switch (elem->get_kind()) {
      case ElementKind::mask:
        model->remove(idx);
        nitems = model->get_n_items();
        return;
      default:
        idx++;
      }
    }
  }
  // Default entries are handled in a manual fashion
}

void ACLListWidget::remove_entry(const Glib::ustring &s, ElementKind e) {
  guint idx, nitems = model->get_n_items();
  for (idx = 0; idx < nitems; idx++) {
    auto elem = model->get_item(idx);
    if (elem->get_kind() == e && elem->get_name() == s) {
      model->remove(idx);
      break;
    }
  }
  remove_unneeded_entries();
}

void ACLListWidget::change_permissions(Glib::RefPtr<ACLItem> item,
                                       Gtk::CheckButton *checkbutton,
                                       const Glib::ustring& permission) {
  // We should not reach here in read only mode.
  g_return_if_fail(!readonly_mode.get_value());

  item->set_property<bool>(permission, checkbutton->get_active());

  controller->update_acl_entry(item->get_kind(), item->get_name(),
                               item->is_read(), item->is_write(),
                               item->is_execute());
}

void ACLListWidget::can_edit_default_acl(bool b) {
  // Show we hide it instead?
  edit_default_participants->set_sensitive(b);
}

void ACLListWidget::default_acl_are_being_edited(bool b) {
  toggling_default_acl = true;
  edit_default_participants->set_active(b);
  toggling_default_acl = false;
}

void ACLListWidget::update_acl_ineffective(
    permissions_t effective_permissions,
    permissions_t effective_default_permissions) {
  bool there_are_ineffective_permissions = false;
  guint idx, end = model->get_n_items();
  for (idx = 0; idx < end; idx++) {
    auto elem = model->get_item(idx);
    switch (elem->get_kind()) {
    case ElementKind::group:
    case ElementKind::acl_user:
    case ElementKind::acl_group:
      elem->set_read_ineffective(!effective_permissions.reading);
      elem->set_write_ineffective(!effective_permissions.writing);
      elem->set_execute_ineffective(!effective_permissions.execution);
      there_are_ineffective_permissions =
          there_are_ineffective_permissions ||
          ((!effective_permissions.reading && elem->is_read()) ||
           (!effective_permissions.writing && elem->is_write()) ||
           (!effective_permissions.execution && elem->is_execute()));
      break;
    case ElementKind::default_group:
    case ElementKind::default_acl_user:
    case ElementKind::default_acl_group:
      elem->set_read_ineffective(!effective_default_permissions.reading);
      elem->set_write_ineffective(!effective_default_permissions.writing);
      elem->set_execute_ineffective(!effective_default_permissions.execution);
      there_are_ineffective_permissions =
          there_are_ineffective_permissions ||
          ((!effective_default_permissions.reading && elem->is_read()) ||
           (!effective_default_permissions.writing && elem->is_write()) ||
           (!effective_default_permissions.execution && elem->is_execute()));
      break;
    // These don't change
    case ElementKind::user:
    case ElementKind::others:
    case ElementKind::mask:
    case ElementKind::default_user:
    case ElementKind::default_mask:
    case ElementKind::default_others:
      break;
    }
  }
  exist_ineffective_permissions.set_value(there_are_ineffective_permissions);
}

void ACLListWidget::toggle_edit_default_acl() {
  // This triggers some nasty reentrancy that we can stop here.
  if (toggling_default_acl)
    return;
  toggling_default_acl = true;

  // Because this is fired after the button has been pressed and released, the
  // value read is always the opposite. Perhaps there is a better way.
  controller->toggle_edit_default_acl(
      !edit_default_participants->get_active(), [this](bool changed) {
        if (!changed) {
          edit_default_participants->set_active(
              !edit_default_participants->get_active());
        }
        toggling_default_acl = false;
      });
}

void ACLListWidget::disable_default_acl_editing() {
  edit_default_participants->set_visible(false);
}

void ACLListWidget::get_textual_representation(Glib::ustring &access_acl,
                                               Glib::ustring &default_acl) {
  access_acl.clear();
  default_acl.clear();

  guint idx, end = model->get_n_items();
  for (idx = 0; idx < end; idx++) {
    auto elem = model->get_item(idx);
    permissions_t p(elem->is_read(), elem->is_write(), elem->is_execute());

    // FIXME: the entry name may not exist and may have to be rendered using
    // (id) syntax
    switch (elem->get_kind()) {
    case ElementKind::user:
      access_acl += "u::" + ACLManager::permission_to_str(p) + "\n";
      break;
    case ElementKind::acl_user:
      access_acl += "u:" + elem->get_name() + ":" +
                    ACLManager::permission_to_str(p) + "\n";
      break;
    case ElementKind::group:
      access_acl += "g::" + ACLManager::permission_to_str(p) + "\n";
      break;
    case ElementKind::acl_group:
      access_acl += "g:" + elem->get_name() + ":" +
                    ACLManager::permission_to_str(p) + "\n";
      break;
    case ElementKind::mask:
      access_acl += "m::" + ACLManager::permission_to_str(p) + "\n";
      break;
    case ElementKind::others:
      access_acl += "o::" + ACLManager::permission_to_str(p) + "\n";
      break;

    case ElementKind::default_user:
      default_acl += "u::" + ACLManager::permission_to_str(p) + "\n";
      break;
    case ElementKind::default_acl_user:
      default_acl += "u:" + elem->get_name() + ":" +
                     ACLManager::permission_to_str(p) + "\n";
      break;
    case ElementKind::default_group:
      default_acl += "g::" + ACLManager::permission_to_str(p) + "\n";
      break;
    case ElementKind::default_acl_group:
      default_acl += "g:" + elem->get_name() + ":" +
                     ACLManager::permission_to_str(p) + "\n";
      break;
    case ElementKind::default_mask:
      default_acl += "m::" + ACLManager::permission_to_str(p) + "\n";
      break;
    case ElementKind::default_others:
      default_acl += "o::" + ACLManager::permission_to_str(p) + "\n";
      break;
    }
  }
}

permissions_t ACLListWidget::get_mask_permissions() {
  permissions_t p(7);
  guint idx, end = model->get_n_items();
  for (idx = 0; idx < end; idx++) {
    auto elem = model->get_item(idx);
    if (elem->get_kind() == ElementKind::mask)
      return permissions_t(elem->is_read(), elem->is_write(),
                           elem->is_execute());
  }
  return p;
}

permissions_t ACLListWidget::get_default_mask_permissions() {
  permissions_t p(7);
  guint idx, end = model->get_n_items();
  for (idx = 0; idx < end; idx++) {
    auto elem = model->get_item(idx);
    if (elem->get_kind() == ElementKind::default_mask)
      return permissions_t(elem->is_read(), elem->is_write(),
                           elem->is_execute());
  }
  return p;
}

void ACLListWidget::remove_button_signal(Gtk::Button *btn) {
  auto it = button_signal_map.find(btn);
  if (it != button_signal_map.end()) {
    it->second.disconnect();
    button_signal_map.erase(it);
  }
}

void ACLListWidget::keep_button_signal(Gtk::Button *btn, sigc::connection c) {
  g_assert(button_signal_map.count(btn) == 0);
  button_signal_map[btn] = c;
}

void ACLListWidget::remove_checkbutton_signal(Gtk::CheckButton *btn) {
  auto it = checkbutton_signal_map.find(btn);
  if (it != checkbutton_signal_map.end()) {
    it->second.disconnect();
    checkbutton_signal_map.erase(it);
  }
}
void ACLListWidget::keep_checkbutton_signal(Gtk::CheckButton *btn,
                                            sigc::connection c) {
  g_assert(checkbutton_signal_map.count(btn) == 0);
  checkbutton_signal_map[btn] = c;
}

} // namespace eiciel
