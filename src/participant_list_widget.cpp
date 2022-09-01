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

#include "eiciel/participant_list_widget.h"
#include <glibmm/binding.h>
#include <grp.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/expression.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/listitem.h>
#include <gtkmm/listview.h>
#include <gtkmm/noselection.h>
#include <gtkmm/signallistitemfactory.h>
#include <gtkmm/stringfilter.h>
#include <pwd.h>

namespace eiciel {

ParticipantListWidget::ParticipantListWidget(
    ParticipantListController *cont, ParticipantListWidgetMode widget_mode)
    : controller(cont), widget_mode(widget_mode) {
  controller->set_view(this);

  // Create UI from Resource
  auto refBuilder = Gtk::Builder::create_from_resource(
      "/org/roger_ferrer/eiciel/participant_list_widget.ui");

  auto main_box = refBuilder->get_object<Gtk::Box>("top-level");
  append(*main_box);

  // Bind widgets
  column_view = refBuilder->get_object<Gtk::ColumnView>("column-view");
  cb_acl_default = refBuilder->get_object<Gtk::CheckButton>("cb-acl-default");

  // Make sure the GType of ParticipantItem has beeen registered.
  { ParticipantItem::create("", ParticipantKind::user); }

  auto filter_name_expr = Gtk::PropertyExpression<Glib::ustring>::create(
      ParticipantItem::get_type(), "name");

  user_model = ParticipantListItemModel::create();

  auto string_filter = Gtk::StringFilter::create(filter_name_expr);
  string_filter->set_match_mode(Gtk::StringFilter::MatchMode::SUBSTRING);
  string_filter->set_search("");

  filter_entry = refBuilder->get_object<Gtk::Entry>("filter-text");
  Glib::Binding::bind_property(filter_entry->property_text(),
                               string_filter->property_search());
  filter_entry->signal_icon_press().connect(
      [this](Gtk::Entry::IconPosition) { filter_entry->set_text(""); });

  filter_user_model = Gtk::FilterListModel::create(user_model, string_filter);

  cb_user = refBuilder->get_object<Gtk::CheckButton>("participant-kind-user");
  cb_user->signal_toggled().connect([this]() {
    fill_user_list();
    participant_kind = ParticipantKind::user;
    column_view->set_model(Gtk::NoSelection::create(filter_user_model));
  });

  group_model = ParticipantListItemModel::create();
  filter_group_model = Gtk::FilterListModel::create(group_model, string_filter);
  cb_group = refBuilder->get_object<Gtk::CheckButton>("participant-kind-group");
  cb_group->signal_toggled().connect([this]() {
    fill_group_list();
    participant_kind = ParticipantKind::group;
    column_view->set_model(Gtk::NoSelection::create(filter_group_model));
  });

  cb_acl_default->signal_toggled().connect([this]() {
    is_default = cb_acl_default->get_active();
    update_default_value_user_list();
    update_default_value_group_list();
  });

  cb_show_system_participants =
      refBuilder->get_object<Gtk::CheckButton>("show-system-participants");
  cb_show_system_participants->signal_toggled().connect([this]() {
    controller->show_system_participants(
        cb_show_system_participants->get_active());
    fill_user_list();
    fill_group_list();
  });

  search_participant =
      refBuilder->get_object<Gtk::Button>("search-participant");
  search_participant->signal_clicked().connect(
      [this]() { do_search_participant(); });

  searched_participant =
      refBuilder->get_object<Gtk::Entry>("searched-participant");
  // Workaround: until 4.8, gtkmm did not wrap the activate signal of Gtk::Entry
  // See: https://gitlab.gnome.org/GNOME/gtkmm/-/issues/100
  // Let's do it the low level way.
  auto cb = [](GtkEntry * /* instance */, ParticipantListWidget *user_data) {
    user_data->do_search_participant();
  };
  // We need to disconnect the signal later, so keep the connection around.
  searched_participant_signal_activate = g_signal_connect(
      /* instance */ searched_participant->gobj(), "activate",
      // Non-capturing lambdas are just functions in disguise
      // so they provide a conversion to a pointer function.
      // Cast it ultimately to Glib's GCallback.
      (GCallback)(void (*)(GtkEntry *, ParticipantListWidget *))cb,
      /* user_data */ this);

  // Column view setup
  Glib::RefPtr<Gtk::ColumnViewColumn> column;

  auto factory_icon = Gtk::SignalListItemFactory::create();
  factory_icon->signal_setup().connect(
      [&](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto image = Gtk::make_managed<Gtk::Image>();
        li->set_child(*image);
        image->set_pixel_size(24);
        // Bind li->"resource-path"->"name" with image->"resource".
        auto label_expr =
            Gtk::PropertyExpression<Glib::RefPtr<Glib::ObjectBase>>::create(
                Gtk::ListItem::get_type(), "item");
        auto icon_name_expr = Gtk::PropertyExpression<Glib::ustring>::create(
            ParticipantItem::get_type(), label_expr, "resource-path");
        icon_name_expr->bind(image->property_resource(), li);
      });

  column = Gtk::ColumnViewColumn::create("", factory_icon);
  column_view->append_column(column);

  auto factory_participant = Gtk::SignalListItemFactory::create();
  factory_participant->signal_setup().connect(
      [](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto label = Gtk::make_managed<Gtk::Label>();
        label->set_xalign(0);
        li->set_child(*label);

        // Bind li->"item"->"name" with label->"label".
        auto label_expr =
            Gtk::PropertyExpression<Glib::RefPtr<Glib::ObjectBase>>::create(
                Gtk::ListItem::get_type(), "item");
        auto name_expr = Gtk::PropertyExpression<Glib::ustring>::create(
            ParticipantItem::get_type(), label_expr, "name");
        name_expr->bind(label->property_label(), li);
      });
  column = Gtk::ColumnViewColumn::create("Participant", factory_participant);
  column_view->append_column(column);

  auto factory_add_button_maker = [&](const Glib::ustring &icon_name,
                                      const Glib::ustring &column_name,
                                      AddParticipantTarget target) {
    auto factory_add_button = Gtk::SignalListItemFactory::create();
    factory_add_button->signal_setup().connect(
        [icon_name, target](const Glib::RefPtr<Gtk::ListItem> &li) {
          auto button = Gtk::make_managed<Gtk::Button>();
          li->set_child(*button);
          button->set_icon_name(icon_name);
          button->set_halign(Gtk::Align::CENTER);

          if (target == AddParticipantTarget::TO_FILE) {
            auto label_expr =
                Gtk::PropertyExpression<Glib::RefPtr<Glib::ObjectBase>>::create(
                    Gtk::ListItem::get_type(), "item");
            auto kind_expr =
                Gtk::PropertyExpression<ParticipantKind>::create(
                    ParticipantItem::get_type(), label_expr, "kind");
            auto false_if_default = Gtk::ClosureExpression<bool>::create(
                [](Glib::RefPtr<Glib::ObjectBase> this_, ParticipantKind k) {
                  return !(k == ParticipantKind::default_user ||
                           k == ParticipantKind::default_group);
                },
                kind_expr);
            false_if_default->bind(button->property_sensitive(), li);
          }
        });
    factory_add_button->signal_bind().connect(
        [this, c = controller, target](const Glib::RefPtr<Gtk::ListItem> &li) {
          auto button = dynamic_cast<Gtk::Button *>(li->get_child());
          if (!button)
            return;
          remove_button_signal(button);
          auto item = li->get_item();
          if (auto participant_item =
                  std::dynamic_pointer_cast<ParticipantItem>(item)) {
            auto sig =
                button->signal_clicked().connect([this, participant_item, c, target]() {
                  ElementKind ek;
                  switch (this->participant_kind) {
                  case ParticipantKind::user:
                  case ParticipantKind::default_user:
                    ek = ElementKind::acl_user;
                    break;
                  case ParticipantKind::group:
                  case ParticipantKind::default_group:
                    ek = ElementKind::acl_group;
                    break;
                  };
                  c->add_acl_entry(target,
                                   participant_item->get_name().c_str(), ek,
                                   is_default);
                });
            keep_button_signal(button, sig);
          }
        });
    factory_add_button->signal_unbind().connect(
        [this](const Glib::RefPtr<Gtk::ListItem> &li) {
          auto button = dynamic_cast<Gtk::Button *>(li->get_child());
          if (!button)
            return;
          remove_button_signal(button);
        });
    column = Gtk::ColumnViewColumn::create(column_name, factory_add_button);
    column_view->append_column(column);
  };

  if (widget_mode == ParticipantListWidgetMode::SINGLE_PANE) {
    factory_add_button_maker("list-add-symbolic", "",
                             AddParticipantTarget::DEFAULT);
  } else if (widget_mode == ParticipantListWidgetMode::DUAL_PANE) {
    factory_add_button_maker("document-new-symbolic", "Add to file ACL",
                             AddParticipantTarget::TO_FILE);
    factory_add_button_maker("folder-new-symbolic", "Add to directory ACL",
                             AddParticipantTarget::TO_DIRECTORY);
  } else {
    g_warn_if_reached();
  }

  is_default = false;
  cb_user->activate();
}

void ParticipantListWidget::update_default_value_model_list(
    Glib::RefPtr<ParticipantListItemModel> current_model) {
  guint idx, n_items = current_model->get_n_items();
  for (idx = 0; idx < n_items; idx++) {
    auto item = current_model->get_item(idx);
    if (is_default) {
      if (item->get_kind() == ParticipantKind::user) {
        item->set_kind(ParticipantKind::default_user);
      } else if (item->get_kind() == ParticipantKind::group) {
        item->set_kind(ParticipantKind::default_user);
      }
    } else {
      if (item->get_kind() == ParticipantKind::default_user) {
        item->set_kind(ParticipantKind::user);
      } else if (item->get_kind() == ParticipantKind::default_group) {
        item->set_kind(ParticipantKind::user);
      }
    }
  }
}

void ParticipantListWidget::update_default_value_user_list() {
  update_default_value_model_list(user_model);
}

void ParticipantListWidget::update_default_value_group_list() {
  update_default_value_model_list(group_model);
}

static int compare_participant_items(const Glib::RefPtr<const eiciel::ParticipantItem> &a,
                      const Glib::RefPtr<const eiciel::ParticipantItem> &b) {

  return a->get_name().compare(b->get_name());
}

void ParticipantListWidget::fill_model_list(
    Glib::RefPtr<ParticipantListItemModel> current_model,
    std::set<Glib::ustring> list, ParticipantKind non_default_kind,
    ParticipantKind default_kind) {
  current_model->remove_all();
  for (auto &i : list) {
    current_model->insert_sorted(ParticipantItem::create(
        i, is_default ? default_kind : non_default_kind),
        sigc::ptr_fun(compare_participant_items));
  }
}

void ParticipantListWidget::fill_user_list() {
  fill_model_list(user_model, controller->get_users_list(),
                  ParticipantKind::user, ParticipantKind::default_user);
}

void ParticipantListWidget::fill_group_list() {
  fill_model_list(group_model, controller->get_groups_list(),
                  ParticipantKind::group, ParticipantKind::default_group);
}

ParticipantListWidget::~ParticipantListWidget() {
  // Cleanup signal connection.
  g_clear_signal_handler(&searched_participant_signal_activate,
                         searched_participant->gobj());
}

void ParticipantListWidget::set_readonly(bool b) { set_sensitive(!b); }

void ParticipantListWidget::can_edit_default_acl(bool b) {
  cb_acl_default->set_sensitive(b);
}

bool ParticipantListWidget::highlight_entry(
    const Glib::ustring &name,
    Glib::RefPtr<ParticipantListItemModel> current_model) {
  guint idx, n_items = current_model->get_n_items();
  bool found = false;
  for (idx = 0; idx < n_items; idx++) {
    auto item = current_model->get_item(idx);
    found = item->get_name() == name;
    if (found)
      break;
  }

  if (!found)
    return false;

  // Adapted from nautilus.
  Gtk::Widget *child = column_view->get_last_child();
  while (child != NULL && !dynamic_cast<Gtk::ListView *>(child)) {
    child = child->get_prev_sibling();
  }

  if (child != NULL) {
    child->activate_action("list.scroll-to-item",
                           Glib::Variant<guint>::create(idx));
  }

  return true;
}

void ParticipantListWidget::do_search_participant() {
  Glib::ustring searched_name = searched_participant->get_text();

  Glib::RefPtr<eiciel::ParticipantListItemModel> current_model;
  if (cb_user->get_active()) {
    current_model = user_model;
  } else if (cb_group->get_active()) {
    current_model = group_model;
  } else {
    g_return_if_reached();
  }

  constexpr Gtk::Entry::IconPosition icon_position =
      Gtk::Entry::IconPosition::SECONDARY;

  bool found = highlight_entry(searched_name, current_model);
  if (found) {
    searched_participant->unset_icon(icon_position);
    searched_participant->set_icon_tooltip_text("", icon_position);
    return;
  }

  bool participant_exists = false;
  if (cb_user->get_active()) {
    participant_exists = controller->lookup_user(searched_name);
    if (participant_exists) {
      user_model->insert_sorted(
          ParticipantItem::create(searched_name,
                                  is_default ? ParticipantKind::default_user
                                             : ParticipantKind::user),
          sigc::ptr_fun(compare_participant_items));
    }
  } else if (cb_group->get_active()) {
    participant_exists = controller->lookup_group(searched_name);
    if (participant_exists) {
      group_model->insert_sorted(
          ParticipantItem::create(searched_name,
                                  is_default ? ParticipantKind::default_group
                                             : ParticipantKind::group),
          sigc::ptr_fun(compare_participant_items));
    }
  } else {
    g_return_if_reached();
  }

  if (!participant_exists) {
    searched_participant->set_icon_from_icon_name("dialog-error-symbolic", icon_position);
    searched_participant->set_icon_activatable(false, icon_position);
    searched_participant->set_icon_tooltip_text(_("Participant not found"), icon_position);
  } else {
    searched_participant->unset_icon(icon_position);
    searched_participant->set_icon_tooltip_text("", icon_position);
    highlight_entry(searched_name, current_model);
  }
}

void ParticipantListWidget::remove_button_signal(Gtk::Button *btn) {
  auto it = button_signal_map.find(btn);
  if (it != button_signal_map.end()) {
    it->second.disconnect();
    button_signal_map.erase(it);
  }
}

void ParticipantListWidget::keep_button_signal(Gtk::Button *btn,
                                               sigc::connection c) {
  g_assert(button_signal_map.count(btn) == 0);
  button_signal_map[btn] = c;
}

} // namespace eiciel
