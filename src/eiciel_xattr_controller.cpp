/*
    Eiciel - GNOME editor of ACL file permissions.
    Copyright (C) 2004-2010 Roger Ferrer Ibáñez

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
	: _xattr_manager(NULL), _window(NULL), _opened_file(false)
{
}

void EicielXAttrController::open_file(const Glib::ustring& filename) throw (XAttrManagerException)
{
	if (_xattr_manager != NULL)
	{
		delete _xattr_manager;
	}

	try
	{
		_xattr_manager = new XAttrManager(filename);
		_opened_file = true;

		_window->set_active(true);

		check_editable();

		_window->fill_attributes(_xattr_manager->get_attributes_list());
	}
	catch (XAttrManagerException e)
	{
		delete _xattr_manager;
		_xattr_manager = NULL;
		_opened_file = false;
		_window->set_active(false);
	}
}

void EicielXAttrController::remove_attribute(const Glib::ustring& nomAtrib)  throw (XAttrManagerException)
{
	_xattr_manager->remove_attribute(nomAtrib);
}

void EicielXAttrController::add_attribute(const Glib::ustring& nomAtrib, const Glib::ustring& valorAtrib) throw (XAttrManagerException)
{
	_xattr_manager->add_attribute(nomAtrib, valorAtrib);
}

void EicielXAttrController::update_attribute_value(const Glib::ustring& nomAtrib, const Glib::ustring& valorNouAtrib) throw (XAttrManagerException) 
{
	_xattr_manager->add_attribute(nomAtrib, valorNouAtrib);
}

void EicielXAttrController::update_attribute_name(const Glib::ustring& nomAnticAtribut, const Glib::ustring& nomNouAtribut) throw (XAttrManagerException)
{
	_xattr_manager->change_attribute_name(nomAnticAtribut, nomNouAtribut);
}

void EicielXAttrController::check_editable()
{
	/*
	 * Comprovem que es el propietari o root
	 */
	uid_t real_user = getuid();
	if ((real_user != 0) && (real_user != _xattr_manager->get_owner_uid()))
	{
		_window->set_readonly(true);
	}
	else
	{
		_window->set_readonly(false);
	}
	
}
