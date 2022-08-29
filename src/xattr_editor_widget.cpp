/*
    Eiciel - GNOME editor of Participant file permissions.
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

#include "eiciel/xattr_editor_controller.h"
#include "eiciel/xattr_editor_widget.h"
#include "eiciel/xattr_item.h"
#include <gtkmm/builder.h>
#include <gtkmm/columnviewcolumn.h>
#include <gtkmm/editablelabel.h>
#include <gtkmm/expression.h>
#include <gtkmm/label.h>
#include <gtkmm/listview.h>
#include <gtkmm/noselection.h>
#include <gtkmm/signallistitemfactory.h>

namespace eiciel {

XAttrEditorWidget::XAttrEditorWidget(XAttrEditorController *controller)
    : controller(controller) {
  controller->set_view(this);

  // Create UI from Resource
  auto refBuilder = Gtk::Builder::create_from_resource(
      "/org/roger_ferrer/eiciel/xattr_editor_widget.ui");

  top_level = refBuilder->get_object<Gtk::Box>("top-level");
  append(*top_level);

  column_view = refBuilder->get_object<Gtk::ColumnView>("editor-columnview");
  entry_name = refBuilder->get_object<Gtk::Entry>("attr-name");
  entry_value = refBuilder->get_object<Gtk::Entry>("attr-value");
  button_add = refBuilder->get_object<Gtk::Button>("button-add");

  auto entry_name_cb = [](GtkEntry * /* instance */,
                          XAttrEditorWidget *user_data) {
    if (user_data->button_add->is_sensitive())
      user_data->entry_value->grab_focus();
  };
  // We need to disconnect the signal later, so keep the connection around.
  entry_name_signal_activate = g_signal_connect(
      /* instance */ entry_name->gobj(), "activate",
      // Non-capturing lambdas are just functions in disguise
      // so they provide a conversion to a pointer function.
      // Cast it ultimately to Glib's GCallback.
      (GCallback)(void (*)(GtkEntry *, XAttrEditorWidget *))entry_name_cb,
      /* user_data */ this);

  auto entry_value_cb = [](GtkEntry * /* instance */,
                           XAttrEditorWidget *user_data) {
    if (user_data->button_add->is_sensitive()) {
      user_data->button_add->activate();
      user_data->entry_name->grab_focus();
    }
  };
  // We need to disconnect the signal later, so keep the connection around.
  entry_value_signal_activate = g_signal_connect(
      /* instance */ entry_value->gobj(), "activate",
      // Non-capturing lambdas are just functions in disguise
      // so they provide a conversion to a pointer function.
      // Cast it ultimately to Glib's GCallback.
      (GCallback)(void (*)(GtkEntry *, XAttrEditorWidget *))entry_value_cb,
      /* user_data */ this);

  model = XAttrListItemModel::create();
  column_view->set_model(Gtk::NoSelection::create(model));

  Glib::RefPtr<Gtk::ColumnViewColumn> column;

  auto factory_name = Gtk::SignalListItemFactory::create();
  factory_name->signal_setup().connect(
      [&](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto label = Gtk::make_managed<Gtk::Label>();
        label->set_xalign(0);
        li->set_child(*label);

        auto text_expr =
            Gtk::PropertyExpression<Glib::RefPtr<Glib::ObjectBase>>::create(
                Gtk::ListItem::get_type(), "item");
        auto name_expr = Gtk::PropertyExpression<Glib::ustring>::create(
            XAttrItem::get_type(), text_expr, "name");
        name_expr->bind(label->property_label(), li);
      });
  column = Gtk::ColumnViewColumn::create("Name", factory_name);
  column_view->append_column(column);

  auto factory_value = Gtk::SignalListItemFactory::create();
  factory_value->signal_setup().connect(
      [&](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto entry = Gtk::make_managed<Gtk::EditableLabel>();
        li->set_child(*entry);

        auto text_expr =
            Gtk::PropertyExpression<Glib::RefPtr<Glib::ObjectBase>>::create(
                Gtk::ListItem::get_type(), "item");
        auto name_expr = Gtk::PropertyExpression<Glib::ustring>::create(
            XAttrItem::get_type(), text_expr, "value");
        name_expr->bind(entry->property_text(), li);
      });
  factory_value->signal_bind().connect(
      [this, c = controller](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto label = dynamic_cast<Gtk::EditableLabel *>(li->get_child());
        if (!label)
          return;
        remove_label_signal(label);
        auto item = li->get_item();
        if (auto participant_item =
                std::dynamic_pointer_cast<XAttrItem>(item)) {
          auto sig = label->signal_changed().connect(
              [this, participant_item, c, label, running = false]() mutable {
                if (running)
                  return;
                running = true;
                Glib::ustring old_value = participant_item->get_value();
                bool result = c->update_attribute_value(
                    participant_item->get_name(), label->get_text());
                if (!result) {
                  // If the update failed, reinstate the old value.
                  label->set_text(old_value);
                }
                running = false;
              });
          keep_label_signal(label, sig);
        }
      });

  column = Gtk::ColumnViewColumn::create("Value", factory_value);
  column_view->append_column(column);

  auto factory_remove_icon = Gtk::SignalListItemFactory::create();
  factory_remove_icon->signal_setup().connect(
      [&](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto button = Gtk::make_managed<Gtk::Button>();
        button->set_icon_name("edit-delete-symbolic");
        li->set_child(*button);
      });
  factory_remove_icon->signal_bind().connect(
      [this, c = controller](const Glib::RefPtr<Gtk::ListItem> &li) {
        auto button = dynamic_cast<Gtk::Button *>(li->get_child());
        if (!button)
          return;
        remove_button_signal(button);
        button->set_visible(!readonly_mode);
        auto item = li->get_item();
        if (auto participant_item =
                std::dynamic_pointer_cast<XAttrItem>(item)) {
          auto sig = button->signal_clicked().connect(
              [this, participant_item, c, li]() {
                bool result = c->remove_attribute(participant_item->get_name());
                if (result) {
                  model->remove(li->get_position());
                }
              });
          keep_button_signal(button, sig);
        }
      });

  column = Gtk::ColumnViewColumn::create("", factory_remove_icon);
  column_view->append_column(column);

  button_add->signal_clicked().connect([this, c = controller] {
    auto name = entry_name->get_text();
    auto value = entry_value->get_text();
    bool result = c->add_attribute(name, value);
    if (result) {
      add_attribute(name, value);
      entry_name->set_text("");
      entry_value->set_text("");
    }
  });

  Glib::Binding::bind_property(
      entry_name->property_text(), button_add->property_sensitive(),
      Glib::Binding::Flags::DEFAULT, [this](const Glib::ustring &name) {
        // FIXME: I assume side-effects here are a bad practice
        bool used = this->attribute_name_is_used(name);
        entry_name->set_icon_from_icon_name(
            used ? "dialog-error-symbolic" : "",
            Gtk::Entry::IconPosition::SECONDARY);
        entry_name->set_icon_tooltip_text(
            used ? "Attribute with this name is already present" : "",
            Gtk::Entry::IconPosition::SECONDARY);
        return !name.empty() && !used;
      });

  entry_name->set_text("");
  set_active(false);
}

XAttrEditorWidget::~XAttrEditorWidget() {
  g_clear_signal_handler(&entry_name_signal_activate,
                         entry_name->gobj());
  g_clear_signal_handler(&entry_value_signal_activate,
                         entry_value->gobj());
}

void XAttrEditorWidget::set_active(bool b) { top_level->set_sensitive(b); }

void XAttrEditorWidget::set_readonly(bool b) {
  readonly_mode = b;
  button_add->set_sensitive(!b);
  entry_name->set_sensitive(!b);
  entry_value->set_sensitive(!b);
}

void XAttrEditorWidget::clear_attributes() { model->remove_all(); }

static int
compare_participant_items(const Glib::RefPtr<const eiciel::XAttrItem> &a,
                          const Glib::RefPtr<const eiciel::XAttrItem> &b) {
  return a->get_name().compare(b->get_name());
}

void XAttrEditorWidget::add_attribute(const Glib::ustring &name,
                                      const Glib::ustring &value) {
  auto new_item = XAttrItem::create(name, value);
  model->insert_sorted(new_item, sigc::ptr_fun(compare_participant_items));
}

void XAttrEditorWidget::remove_button_signal(Gtk::Button *btn) {
  auto it = button_signal_map.find(btn);
  if (it != button_signal_map.end()) {
    it->second.disconnect();
    button_signal_map.erase(it);
  }
}

void XAttrEditorWidget::keep_button_signal(Gtk::Button *btn,
                                           sigc::connection c) {
  g_assert(button_signal_map.count(btn) == 0);
  button_signal_map[btn] = c;
}

void XAttrEditorWidget::remove_label_signal(Gtk::EditableLabel *btn) {
  auto it = label_signal_map.find(btn);
  if (it != label_signal_map.end()) {
    it->second.disconnect();
    label_signal_map.erase(it);
  }
}

void XAttrEditorWidget::keep_label_signal(Gtk::EditableLabel *btn,
                                          sigc::connection c) {
  g_assert(label_signal_map.count(btn) == 0);
  label_signal_map[btn] = c;
}

bool XAttrEditorWidget::attribute_name_is_used(const Glib::ustring &name) {
  guint idx, end = model->get_n_items();
  for (idx = 0; idx < end; idx++) {
    auto elem = model->get_item(idx);
    if (elem->get_name() == name) {
      return true;
    }
  }
  return false;
}

} // namespace eiciel
