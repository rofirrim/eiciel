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

#ifndef EICIEL_ACL_ITEM_H
#define EICIEL_ACL_ITEM_H

#include "eiciel/acl_element_kind.h"
#include <giomm/liststore.h>
#include <glibmm/binding.h>
#include <glibmm/object.h>
#include <glibmm/property.h>
#include <glibmm/ustring.h>
#include <tuple>

namespace eiciel {

struct ACLItem : public Glib::Object {
public:
  static Glib::RefPtr<ACLItem> create(ElementKind kind,
                                      const Glib::ustring &name, bool r, bool w,
                                      bool x, bool removable) {
    return Glib::make_refptr_for_instance<ACLItem>(
        new ACLItem{kind, name, r, w, x, removable});
  }

  Glib::ustring get_name() const { return name.get_value(); }
  ElementKind get_kind() const { return kind; }
  bool is_read() const { return r.get_value(); }
  bool is_write() const { return w.get_value(); }
  bool is_execute() const { return x.get_value(); }
  bool can_be_removed() const { return removable.get_value(); }

  void set_read(bool b) { r.set_value(b); }
  void set_write(bool b) { w.set_value(b); }
  void set_execute(bool b) { x.set_value(b); }

  bool is_read_ineffective() const { return ineffective_r.get_value(); }
  bool is_write_ineffective() const { return ineffective_w.get_value(); }
  bool is_execute_ineffective() const { return ineffective_x.get_value(); }

  void set_read_ineffective(bool b) { ineffective_r.set_value(b); }
  void set_write_ineffective(bool b) { ineffective_w.set_value(b); }
  void set_execute_ineffective(bool b) { ineffective_x.set_value(b); }

  // No idea why the ObjectBase::get_type won't work for us but
  // reintroducing the method and using the name used by gtkmm seems
  // to work.
  static GType get_type() {
    // Let's cache once the type does exist.
    if (!gtype)
      gtype = g_type_from_name("gtkmm__CustomObject_ACLItem");
    return gtype;
  }

  Glib::PropertyProxy_ReadOnly<bool> property_masked_r() const {
    return masked_r.get_proxy();
  }
  Glib::PropertyProxy_ReadOnly<bool> property_masked_w() const {
    return masked_w.get_proxy();
  }
  Glib::PropertyProxy_ReadOnly<bool> property_masked_x() const {
    return masked_x.get_proxy();
  }

  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_resource_path() const {
    return resource_path.get_proxy();
  }

private:
  ACLItem(ElementKind kind, const Glib::ustring &str, bool r, bool w, bool x,
          bool removable)
      : Glib::ObjectBase("ACLItem"), kind(*this, "kind", kind),
        name(*this, "name", str), r(*this, "r", r), w(*this, "w", w),
        x(*this, "x", x), removable(*this, "removable", removable),
        ineffective_r(*this, "ineffective_r", false),
        ineffective_w(*this, "ineffective_w", false),
        ineffective_x(*this, "ineffective_x", false),
        masked_r(*this, "masked_r", false), masked_w(*this, "masked_w", false),
        masked_x(*this, "masked_x", false),
        resource_path(*this, "resource-path", compute_icon_resource(kind)) {
    Glib::Binding::bind_property(
        this->r.get_proxy(), this->masked_r.get_proxy(),
        Glib::Binding::Flags::DEFAULT, [this](bool b) { return b && this->is_read_ineffective(); });
    Glib::Binding::bind_property(
        this->ineffective_r.get_proxy(), this->masked_r.get_proxy(),
        Glib::Binding::Flags::DEFAULT, [this](bool b) { return b && this->is_read(); });

    Glib::Binding::bind_property(
        this->w.get_proxy(), this->masked_w.get_proxy(),
        Glib::Binding::Flags::DEFAULT, [this](bool b) { return b && this->is_write_ineffective(); });
    Glib::Binding::bind_property(
        this->ineffective_w.get_proxy(), this->masked_w.get_proxy(),
        Glib::Binding::Flags::DEFAULT, [this](bool b) { return b && this->is_write(); });

    Glib::Binding::bind_property(
        this->x.get_proxy(), this->masked_x.get_proxy(),
        Glib::Binding::Flags::DEFAULT, [this](bool b) { return b && this->is_execute_ineffective(); });
    Glib::Binding::bind_property(
        this->ineffective_x.get_proxy(), this->masked_x.get_proxy(),
        Glib::Binding::Flags::DEFAULT, [this](bool b) { return b && this->is_execute(); });
  }
  Glib::Property<ElementKind> kind;
  Glib::Property<Glib::ustring> name;
  Glib::Property<bool> r, w, x;
  Glib::Property<bool> removable;
  Glib::Property<bool> ineffective_r, ineffective_w, ineffective_x;
  Glib::Property<bool> masked_r, masked_w, masked_x;
  Glib::Property<Glib::ustring> resource_path;

  static Glib::ustring compute_icon_resource(ElementKind kind) {
    Glib::ustring resource_name = "/org/roger_ferrer/eiciel/";
    switch (kind) {
    case ElementKind::user:
      resource_name += "user.png";
      break;
    case ElementKind::acl_user:
      resource_name += "user-acl.png";
      break;
    case ElementKind::group:
      resource_name += "group.png";
      break;
    case ElementKind::acl_group:
      resource_name += "group-acl.png";
      break;
    case ElementKind::others:
      resource_name += "others.png";
      break;
    case ElementKind::mask:
      resource_name += "mask.png";
      break;
    case ElementKind::default_user:
      resource_name += "user-default.png";
      break;
    case ElementKind::default_acl_user:
      resource_name += "user-acl-default.png";
      break;
    case ElementKind::default_group:
      resource_name += "group-default.png";
      break;
    case ElementKind::default_acl_group:
      resource_name += "group-acl-default.png";
      break;
    case ElementKind::default_others:
      resource_name += "others-default.png";
      break;
    case ElementKind::default_mask:
      resource_name += "mask-default.png";
      break;
    default:
      g_warn_if_reached();
      resource_name = "";
      break;
    }
    return resource_name;
  }

  static GType gtype;
};

using ACLListItemModel = Gio::ListStore<ACLItem>;

} // namespace eiciel

#endif // EICIEL_ACL_ITEM_H