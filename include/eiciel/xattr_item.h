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

#ifndef EICIEL_XATTR_ITEM_H
#define EICIEL_XATTR_ITEM_H

#include "config.h"
#include <giomm/liststore.h>
#include <glibmm/object.h>
#include <glibmm/property.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>

namespace eiciel {

class XAttrItem : public Glib::Object {
public:
  static Glib::RefPtr<XAttrItem> create(const Glib::ustring &name,
                                        const Glib::ustring &value) {
    return Glib::make_refptr_for_instance<XAttrItem>(new XAttrItem{name, value});
  }

  Glib::ustring get_name() const { return name.get_value(); }
  Glib::ustring get_value() const { return value.get_value(); }

  // No idea why the ObjectBase::get_type won't work for us but
  // reintroducing the method and using the name used by gtkmm seems
  // to work.
  static GType get_type() {
    // Let's cache once the type does exist.
    if (!gtype)
      gtype = g_type_from_name("gtkmm__CustomObject_XAttrItem");
    return gtype;
  }

private:
  XAttrItem(const Glib::ustring &name, const Glib::ustring &value)
      : Glib::ObjectBase("XAttrItem"), name(*this, "name", name),
        value(*this, "value", value) {}

  Glib::Property<Glib::ustring> name;
  Glib::Property<Glib::ustring> value;

  static GType gtype;
};

using XAttrListItemModel = Gio::ListStore<XAttrItem>;

} // namespace eiciel

#endif