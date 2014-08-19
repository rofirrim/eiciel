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
#ifndef TIPUS_ELEMENT_ACL_HPP 
#define TIPUS_ELEMENT_ACL_HPP
#include <config.hpp>

enum ElementKind 
{ 
    EK_USER,
    EK_GROUP,
    EK_OTHERS,

    EK_ACL_USER,
    EK_ACL_GROUP,
    EK_MASK,

    EK_DEFAULT_USER,
    EK_DEFAULT_GROUP,
    EK_DEFAULT_OTHERS,

    EK_DEFAULT_ACL_USER,
    EK_DEFAULT_ACL_GROUP,
    EK_DEFAULT_MASK 
};

enum PermissionKind 
{ 
    PK_READING,
    PK_WRITING,
    PK_EXECUTION 
};

#endif
