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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,  USA
*/

#ifndef EICIEL_XATTR_MANAGER_H
#define EICIEL_XATTR_MANAGER_H

#include "config.h"

#include "eiciel/i18n.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/xattr.h>
#ifndef ENOATTR
#define ENOATTR ENODATA
#endif

#include <cstring>
#include <glibmm.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace eiciel {

class XAttrManagerException {
public:
    XAttrManagerException(Glib::ustring missatge)
        : missatge(missatge)
    {
    }

    Glib::ustring getMessage() const { return missatge; }

private:
    Glib::ustring missatge;
};

class XAttrManager {
public:
    typedef std::map<std::string, std::string> attributes_t;

    XAttrManager(const Glib::ustring& filename);

    attributes_t get_attributes_list();
    void remove_attribute(const std::string& attr_name);
    void set_attribute(attributes_t atributes);
    void add_attribute(const std::string& attr_name,
        const std::string& attr_value);
    void change_attribute_name(const std::string& old_attr_name,
        const std::string& new_attr_name);

    uid_t get_owner_uid() { return this->_owner; }

private:
    std::vector<std::string> get_xattr_list();
    std::string get_attribute_value(const std::string& attr_name);

    void read_test();

    Glib::ustring _filename;

    uid_t _owner;
};

}

#endif // EICIEL_XATTR_MANAGER_H
