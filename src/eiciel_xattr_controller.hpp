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
#ifndef EICIEL_XATTR_CONTROLER_HPP
#define EICIEL_XATTR_CONTROLER_HPP

#include <sys/types.h>
#include "eiciel_xattr_window.hpp"
#include "xattr_manager.hpp"

class EicielXAttrWindow;

class EicielXAttrController : public sigc::trackable
{
    private:
        XAttrManager* _xattr_manager;
        EicielXAttrWindow* _window;
        bool _opened_file;

        void remove_attribute(const Glib::ustring& attrib_name) throw (XAttrManagerException);

        void add_attribute(const Glib::ustring& attrib_name, 
                const Glib::ustring& attrib_value) throw (XAttrManagerException);
        void update_attribute_value(const Glib::ustring& attrib_name, 
                const Glib::ustring& attrib_new_value) throw (XAttrManagerException) ;
        void update_attribute_name(const Glib::ustring& old_attribute_name, 
                const Glib::ustring& new_attribute_name) throw (XAttrManagerException);

        void check_editable();
    public:
        EicielXAttrController();
        bool opened_file() { return _opened_file; };

        void open_file(const Glib::ustring& filename) throw (XAttrManagerException);

        friend class EicielXAttrWindow;
}
;

#endif
