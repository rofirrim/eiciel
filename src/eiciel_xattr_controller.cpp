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

#include "eiciel_xattr_controller.hpp"

EicielXAttrController::EicielXAttrController()
    : _xattr_manager(NULL)
    , _window(NULL)
    , _opened_file(false)
{
}

void EicielXAttrController::open_file(const Glib::ustring& filename)
{
    if (_xattr_manager != NULL) {
        delete _xattr_manager;
        _xattr_manager = NULL;
    }

    try {
        _xattr_manager = new XAttrManager(filename);
        _opened_file = true;

        set_active(true);

        check_editable();

        _window->fill_attributes(_xattr_manager->get_attributes_list());
    } catch (XAttrManagerException e) {
        delete _xattr_manager;
        _xattr_manager = NULL;
        _opened_file = false;
        set_active(false);
    }
}

void EicielXAttrController::remove_attribute(const Glib::ustring& attrib_name)
{
    _xattr_manager->remove_attribute(attrib_name);
}

void EicielXAttrController::add_attribute(
    const Glib::ustring& attrib_name,
    const Glib::ustring& attrib_value)
{
    _xattr_manager->add_attribute(attrib_name, attrib_value);
}

void EicielXAttrController::update_attribute_value(
    const Glib::ustring& attrib_name,
    const Glib::ustring& attrib_new_value)
{
    _xattr_manager->add_attribute(attrib_name, attrib_new_value);
}

void EicielXAttrController::update_attribute_name(
    const Glib::ustring& old_attribute_name,
    const Glib::ustring& new_attribute_name)
{
    _xattr_manager->change_attribute_name(old_attribute_name, new_attribute_name);
}

void EicielXAttrController::check_editable()
{
    // Check that it is the owner or root.
    uid_t real_user = getuid();
    if ((real_user != 0) && (real_user != _xattr_manager->get_owner_uid())) {
        _window->set_readonly(true);
    } else {
        _window->set_readonly(false);
    }
}

void EicielXAttrController::set_active(bool b)
{
    _window->set_active(b);
}
