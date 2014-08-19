/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2004-2014 Roger Ferrer Ibáñez

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

#ifndef GESTOR_XATTR_HPP
#define GESTOR_XATTR_HPP

#include <config.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <attr/xattr.h>

#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <glibmm.h>
#include <glib/gi18n-lib.h>
#include <iostream>


class XAttrManagerException
{
    public:
        XAttrManagerException(Glib::ustring missatge) : missatge(missatge) 
        {
        }

        Glib::ustring getMessage() const
        { 
            return missatge; 
        }
    private:
        Glib::ustring missatge;
};

class XAttrManager {

    public:
        typedef std::map<std::string, std::string> attributes_t;

        XAttrManager(const Glib::ustring& filename) throw (XAttrManagerException);

        attributes_t get_attributes_list();
        void remove_attribute(std::string attr_name);
        void set_attribute(attributes_t atributes);
        void add_attribute(std::string attr_name, std::string attr_value);
        void change_attribute_name(std::string old_attr_name, std::string new_attr_name);

        uid_t get_owner_uid()
        {
            return this->_owner;
        }

    private:
        std::vector<std::string> get_xattr_list() throw (XAttrManagerException);
        std::string get_attribute_value(const std::string& attr_name) throw (XAttrManagerException);

        void read_test() throw (XAttrManagerException);

        Glib::ustring _filename;

        uid_t _owner;
}
;

#endif
