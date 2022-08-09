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

#ifndef EICIEL_PARTICIPANT_ITEM_H
#define EICIEL_PARTICIPANT_ITEM_H

#include <giomm/liststore.h>
#include <glibmm/binding.h>
#include <glibmm/object.h>
#include <glibmm/property.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>

namespace eiciel {

enum class ParticipantKind { user, group, default_user, default_group };

class ParticipantItem : public Glib::Object {
public:
  static Glib::RefPtr<ParticipantItem> create(const Glib::ustring &name,
                                              ParticipantKind kind) {
    return Glib::make_refptr_for_instance<ParticipantItem>(
        new ParticipantItem{name, kind});
  }

  Glib::ustring get_name() const { return name.get_value(); }

  ParticipantKind get_kind() const { return kind.get_value(); }
  void set_kind(ParticipantKind k) { kind.set_value(k); }

  static GType get_type() {
    // Let's cache once the type does exist.
    if (!gtype)
      gtype = g_type_from_name("gtkmm__CustomObject_ParticipantItem");
    return gtype;
  }

  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_resource_path() const {
    return resource_path.get_proxy();
  }

private:
  ParticipantItem(const Glib::ustring &name, ParticipantKind kind)
      : Glib::ObjectBase("ParticipantItem"), name(*this, "name", name),
        kind(*this, "kind", kind),
        resource_path(*this, "resource-path", compute_icon_resource(kind)) {

    Glib::Binding::bind_property(
        this->kind.get_proxy(), this->resource_path.get_proxy(),
        Glib::Binding::Flags::DEFAULT,
        [](ParticipantKind k) { return compute_icon_resource(k); });
  }

  static Glib::ustring compute_icon_resource(ParticipantKind k) {
    Glib::ustring resource_name = "/org/roger_ferrer/eiciel/";
    switch (k) {
    case ParticipantKind::user:
      resource_name += "user-acl.png";
      break;
    case ParticipantKind::default_user:
      resource_name += "user-acl-default.png";
      break;
    case ParticipantKind::group:
      resource_name += "group-acl.png";
      break;
    case ParticipantKind::default_group:
      resource_name += "group-acl-default.png";
      break;
    default:
      g_warn_if_reached();
      resource_name = "";
      break;
    }
    return resource_name;
  }

  Glib::Property<Glib::ustring> name;
  Glib::Property<ParticipantKind> kind;
  Glib::Property<Glib::ustring> resource_path;

  static GType gtype;
};

using ParticipantListItemModel = Gio::ListStore<ParticipantItem>;

} // namespace eiciel

#endif // EICIEL_PARTICIPANT_ITEM_H