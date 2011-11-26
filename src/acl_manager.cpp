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
#include "acl_manager.hpp"


ACLManager::ACLManager(const string& filename) throw (ACLManagerException)
	: _filename(filename) 
{
	// Get first UGO permissions and info about the file
	get_ugo_permissions();

	get_acl_entries_access();

	// If it is a directory get default acl entries
	if (_is_directory)
	{
		get_acl_entries_default();
	}

	create_textual_representation();
}

void ACLManager::get_ugo_permissions()
{
	struct stat buffer;
	if (stat(_filename.c_str(), &buffer) == -1)
	{
		throw ACLManagerException(Glib::locale_to_utf8(strerror(errno)));
	}

	// Check if a regular file or directory
	if (!S_ISREG(buffer.st_mode) && !S_ISDIR(buffer.st_mode))
	{
		throw ACLManagerException(_("Only regular files or directories supported")); 
	}

	// Save whether is a directory
	_is_directory = S_ISDIR(buffer.st_mode);

	// Get the user name
	_uid_owner = buffer.st_uid;
	struct passwd* u = getpwuid(buffer.st_uid);

	if (u == NULL)
	{
		stringstream ss;
		ss << "(" << buffer.st_uid << ")"; 
		_owner_name = ss.str();
	}
	else
	{
		_owner_name = u->pw_name;
	}

	// Get the group name
	struct group* g = getgrgid(buffer.st_gid);

	if (g == NULL)
	{
		stringstream ss;
		ss << "(" << buffer.st_gid << ")"; 
		_group_name = ss.str();
	}
	else
	{
		_group_name = g->gr_name;
	}
}


void ACLManager::get_acl_entries_access()
{
	_user_acl.clear();
	_group_acl.clear();
	_there_is_mask = false;
	// Get access ACL
	acl_t acl_file = acl_get_file(_filename.c_str(), ACL_TYPE_ACCESS);

    if (acl_file == (acl_t) NULL)
    {
        throw ACLManagerException(Glib::locale_to_utf8(strerror(errno)));
    }

	// Get all the entries
	acl_entry_t acl_entry_;
	acl_permset_t permission_set;
	acl_tag_t acl_kind_tag;

	int found = acl_get_entry(acl_file, ACL_FIRST_ENTRY, &acl_entry_);
	while (found == 1)
	{
		acl_get_permset(acl_entry_, &permission_set);
		acl_get_tag_type(acl_entry_, &acl_kind_tag);

		if (acl_kind_tag == ACL_USER || acl_kind_tag == ACL_GROUP)
		{
			// A user|group entry
			// Gather the permissions
			acl_entry new_acl;
			new_acl.reading = ACL_GET_PERM(permission_set, ACL_READ);
			new_acl.writing = ACL_GET_PERM(permission_set, ACL_WRITE);
			new_acl.execution = ACL_GET_PERM(permission_set, ACL_EXECUTE);
			// Get the qualifier
			if (acl_kind_tag == ACL_USER)
			{
				void* ptr_acluser = acl_get_qualifier(acl_entry_);
				uid_t* iduser = (uid_t*) ptr_acluser;
				struct passwd* p = getpwuid(*iduser);
				new_acl.valid_name = (p != NULL);
				if (p == NULL) 
				{
					stringstream ss;
					ss << "(" << *iduser << ")";
					new_acl.name = ss.str();
				}
				else 
				{
					new_acl.name = p->pw_name;
				}
				new_acl.qualifier = *iduser;
				acl_free(ptr_acluser);

				_user_acl.push_back(new_acl);
			}
			else
			{
				void* ptr_aclgroup = acl_get_qualifier(acl_entry_);
				gid_t* idgroup = (gid_t*) ptr_aclgroup;
				struct group* g = getgrgid(*idgroup);
				new_acl.valid_name = (g != NULL);
				if (g == NULL)
				{
					stringstream ss;
					ss << "(" << *idgroup << ")";
					new_acl.name = ss.str();
				}
				else
				{
					new_acl.name = g->gr_name;
				}
				new_acl.qualifier = *idgroup;
				acl_free(ptr_aclgroup);

				_group_acl.push_back(new_acl);
			}
		}
		else if (acl_kind_tag == ACL_MASK)
		{
			// The ACL mask
			_there_is_mask = true;
			_mask_acl.reading = ACL_GET_PERM(permission_set, ACL_READ);
			_mask_acl.writing = ACL_GET_PERM(permission_set, ACL_WRITE);
			_mask_acl.execution = ACL_GET_PERM(permission_set, ACL_EXECUTE);
		}
		else if (acl_kind_tag == ACL_USER_OBJ)
		{
            // Owner
			_owner_perms.reading = ACL_GET_PERM(permission_set, ACL_READ);
			_owner_perms.writing = ACL_GET_PERM(permission_set, ACL_WRITE);
			_owner_perms.execution = ACL_GET_PERM(permission_set, ACL_EXECUTE);
			
		}
		else if (acl_kind_tag == ACL_GROUP_OBJ)
		{
            // Group
			_group_perms.reading = ACL_GET_PERM(permission_set, ACL_READ);
			_group_perms.writing = ACL_GET_PERM(permission_set, ACL_WRITE);
			_group_perms.execution = ACL_GET_PERM(permission_set, ACL_EXECUTE);

		}
		else if (acl_kind_tag == ACL_OTHER)
		{
            // Other
			_others_perms.reading = ACL_GET_PERM(permission_set, ACL_READ);
			_others_perms.writing = ACL_GET_PERM(permission_set, ACL_WRITE);
			_others_perms.execution = ACL_GET_PERM(permission_set, ACL_EXECUTE);
		}

		found = acl_get_entry(acl_file, ACL_NEXT_ENTRY, &acl_entry_);
	}

	acl_free(acl_file);
}

void ACLManager::get_acl_entries_default()
{
	_there_is_default_mask 
        = _there_is_default_user 
        = _there_is_default_group 
        = _there_is_default_others 
        = false;

    _default_user_acl.clear();
    _default_group_acl.clear();

	acl_t acl_file = acl_get_file(_filename.c_str(), ACL_TYPE_DEFAULT);

	// Get all the entries
	acl_entry_t acl_entry_;
	acl_permset_t permission_set;
	acl_tag_t acl_kind_tag;

	int found = acl_get_entry(acl_file, ACL_FIRST_ENTRY, &acl_entry_);
	while (found == 1)
	{
		acl_get_permset(acl_entry_, &permission_set);
		acl_get_tag_type(acl_entry_, &acl_kind_tag);

		if (acl_kind_tag == ACL_USER || acl_kind_tag == ACL_GROUP)
		{
			// An entry of type user/group
            // get all permissions
			acl_entry new_acl;
			new_acl.reading = ACL_GET_PERM(permission_set, ACL_READ);
			new_acl.writing = ACL_GET_PERM(permission_set, ACL_WRITE);
			new_acl.execution = ACL_GET_PERM(permission_set, ACL_EXECUTE);
			// Get the qualifiers
			if (acl_kind_tag == ACL_USER)
			{
				void* ptr_iduser = acl_get_qualifier(acl_entry_);
				uid_t* iduser = (uid_t*)ptr_iduser;
				struct passwd* p = getpwuid(*iduser);
				new_acl.valid_name = (p != NULL);
				if (p == NULL) 
				{
					stringstream ss;
					ss << "(" << *iduser << ")";
					new_acl.name = ss.str();
				}
				else 
				{
					new_acl.name = p->pw_name;
				}
				new_acl.qualifier = *iduser;
				acl_free(ptr_iduser);

				_default_user_acl.push_back(new_acl);
			}
			else
			{
				void* ptr_idgroup = acl_get_qualifier(acl_entry_);
				gid_t* idgroup = (gid_t*)ptr_idgroup;
				struct group* g = getgrgid(*idgroup);
				new_acl.valid_name = (g != NULL);
				if (g == NULL)
				{
					stringstream ss;
					ss << "(" << *idgroup << ")";
					new_acl.name = ss.str();
				}
				else
				{
					new_acl.name = g->gr_name;
				}
				new_acl.qualifier = *idgroup;
				acl_free(ptr_idgroup);

				_default_group_acl.push_back(new_acl);
			}
		}
		else if (acl_kind_tag == ACL_USER_OBJ)
		{
			// Default user entry
			_there_is_default_user = true;
			_default_user.reading = ACL_GET_PERM(permission_set, ACL_READ);
			_default_user.writing = ACL_GET_PERM(permission_set, ACL_WRITE);
			_default_user.execution = ACL_GET_PERM(permission_set, ACL_EXECUTE);
		}
		else if (acl_kind_tag == ACL_GROUP_OBJ)
		{
			// Default group entry
			_there_is_default_group = true;
			_default_group.reading = ACL_GET_PERM(permission_set, ACL_READ);
			_default_group.writing = ACL_GET_PERM(permission_set, ACL_WRITE);
			_default_group.execution = ACL_GET_PERM(permission_set, ACL_EXECUTE);
		}
		else if (acl_kind_tag == ACL_OTHER)
		{
			// Default others entry
			_there_is_default_others = true;
			_default_others.reading = ACL_GET_PERM(permission_set, ACL_READ);
			_default_others.writing = ACL_GET_PERM(permission_set, ACL_WRITE);
			_default_others.execution = ACL_GET_PERM(permission_set, ACL_EXECUTE);
		}
		else if (acl_kind_tag == ACL_MASK)
		{
            // Default mask
			_there_is_default_mask = true;
			_default_mask.reading = ACL_GET_PERM(permission_set, ACL_READ);
			_default_mask.writing = ACL_GET_PERM(permission_set, ACL_WRITE);
			_default_mask.execution = ACL_GET_PERM(permission_set, ACL_EXECUTE);
		}

		found = acl_get_entry(acl_file, ACL_NEXT_ENTRY, &acl_entry_);
	}

	acl_free(acl_file);
}

void ACLManager::create_textual_representation()
{
	_text_acl_access.clear();

	_text_acl_access += "u::" + permission_to_str(_owner_perms) + "\n";
	for (vector<acl_entry>::iterator i = _user_acl.begin();
			i != _user_acl.end(); i++)
	{
		_text_acl_access += "u:" + write_name(*i) + ":" + permission_to_str(*i) + "\n";
	}
	
	_text_acl_access += "g::" + permission_to_str(_group_perms) + "\n";
	for (vector<acl_entry>::iterator i = _group_acl.begin();
			i != _group_acl.end(); i++)
	{
		_text_acl_access += "g:" + write_name(*i) + ":" + permission_to_str(*i) + "\n";
	}

	if (_there_is_mask)
	{
		_text_acl_access += "m::" + permission_to_str(_mask_acl) + "\n";
	}
	_text_acl_access += "o::" + permission_to_str(_others_perms) + "\n";

	_text_acl_default.clear();
	if (_is_directory)
	{
		if (_there_is_default_user)
		{
			_text_acl_default += "u::" + permission_to_str(_default_user) + "\n";
		}
		if (_there_is_default_group)
		{
			_text_acl_default += "g::" + permission_to_str(_default_group) + "\n";
		}
		if (_there_is_default_others)
		{
			_text_acl_default += "o::" + permission_to_str(_default_others) + "\n";
		}

		for (vector<acl_entry>::iterator i = _default_user_acl.begin();
				i != _default_user_acl.end(); i++)
		{
			_text_acl_default += "u:" + write_name(*i) + ":" + permission_to_str(*i) + "\n";
		}
		for (vector<acl_entry>::iterator i = _default_group_acl.begin();
				i != _default_group_acl.end(); i++)
		{
			_text_acl_default += "g:" + write_name(*i) + ":" + permission_to_str(*i) + "\n";
		}

		if (_there_is_default_mask)
		{
			_text_acl_default += "m::" + permission_to_str(_default_mask) + "\n";
		}
	}
}

string ACLManager::write_name(acl_entry& eacl)
{
	if (eacl.valid_name)
	{
		return eacl.name;
	}
	else
	{
		stringstream ss;
		ss << eacl.qualifier;
		return ss.str();
	}
}

string ACLManager::permission_to_str(permissions_t& p)
{
	string s;
	s.clear();
	s += (p.reading ? "r" : "-");
	s += (p.writing ? "w" : "-");
	s += (p.execution ? "x" : "-");
	return s;
}

void ACLManager::modify_acl_user(const string& username, const permissions_t& perms)
{
	set_acl_generic(username, _user_acl, perms);
	update_changes_acl_access();
}

void ACLManager::modify_acl_group(const string& groupname, const permissions_t& perms)
{
	set_acl_generic(groupname, _group_acl, perms);
	update_changes_acl_access();
}

void ACLManager::update_changes_acl_access()
{
    // If there is any entry of _user_acl or _group_acl we need a mask
	if ((_user_acl.size() + _group_acl.size()) > 0)
	{
		if (!_there_is_mask)
		{
			calculate_access_mask();
		}
	}
	else // Otherwise it is not needed
	{
		_there_is_mask = false;
	}
	create_textual_representation();
	commit_changes_to_file();
}

void ACLManager::modify_acl_default_user(const string& username, const permissions_t& perms)
{
	set_acl_generic(username, _default_user_acl, perms);
    update_changes_acl_default();
}

void ACLManager::modify_acl_default_group(const string& groupname, const permissions_t& perms)
{
	set_acl_generic(groupname, _default_group_acl, perms);
	update_changes_acl_default();
}

void ACLManager::update_changes_acl_default()
{
	if ((_default_user_acl.size() + _default_group_acl.size()) > 0)
	{
		fill_needed_acl_default();
	}

	create_textual_representation();
	commit_changes_to_file();
}

void ACLManager::fill_needed_acl_default()
{
	if (!_there_is_default_user)
	{
		_there_is_default_user = true;
		_default_user.reading = _owner_perms.reading;
		_default_user.writing = _owner_perms.writing;
		_default_user.execution = _owner_perms.execution;
	}
	if (!_there_is_default_group)
	{
		_there_is_default_group = true;
		_default_group.reading = _group_perms.reading;
		_default_group.writing = _group_perms.writing;
		_default_group.execution = _group_perms.execution;
	}
	if (!_there_is_default_others)
	{
		_there_is_default_others = true;
		_default_others.reading = _others_perms.reading;
		_default_others.writing = _others_perms.writing;
		_default_others.execution = _others_perms.execution;
	}
	if (!_there_is_default_mask)
	{
		_there_is_default_mask = true;
		_default_mask = permissions_t(7);
	}
}

void ACLManager::set_acl_generic(const string& name, vector<acl_entry>& acl_list, 
        const permissions_t& perms)
{
	ACLEquivalence equiv_acl(name);
	vector<acl_entry>::iterator i = find_if(acl_list.begin(), acl_list.end(), equiv_acl);
	if (i != acl_list.end()) // If already there, update
	{
		i->reading = perms.reading;
		i->writing = perms.writing;
		i->execution = perms.execution;
	}
	else // If not there, create
	{
		acl_entry eacl;
		eacl.valid_name = true;
		eacl.name = name;
		eacl.reading = perms.reading;
		eacl.writing = perms.writing;
		eacl.execution = perms.execution;
		acl_list.push_back(eacl);
	}
}

void ACLManager::remove_acl_user(const string& username)
{
	remove_acl_generic(username, _user_acl);
	update_changes_acl_access();
}

void ACLManager::remove_acl_group(const string& groupname)
{
	remove_acl_generic(groupname, _group_acl);
	update_changes_acl_access();
}

void ACLManager::remove_acl_user_default(const string& username)
{
	remove_acl_generic(username, _default_user_acl);
	update_changes_acl_default();
}

void ACLManager::remove_acl_group_default(const string& groupname)
{
	remove_acl_generic(groupname, _default_group_acl);
	update_changes_acl_default();
}

void ACLManager::remove_acl_generic(const string& name, vector<acl_entry>& acl_list)
{
	ACLEquivalence equiv_acl(name);
	acl_list.erase(remove_if(acl_list.begin(), acl_list.end(), equiv_acl), acl_list.end());
}

void ACLManager::commit_changes_to_file()
{
	// Get the textual representation of the ACL
	acl_t acl_access = acl_from_text(_text_acl_access.c_str());
	if (acl_access == NULL)
	{
		std::cerr << "ACL is wrong!!!" << endl << _text_acl_access.c_str() << endl;
		
		throw ACLManagerException(_("Textual representation of the ACL is wrong"));
	}
    if (acl_set_file(_filename.c_str(), ACL_TYPE_ACCESS, acl_access) != 0)
	{
		throw ACLManagerException(Glib::locale_to_utf8(strerror(errno)));
	}

	if (_is_directory)
	{
		// Clear the ACL
		if (acl_delete_def_file(_filename.c_str()) != 0)
		{
			throw ACLManagerException(Glib::locale_to_utf8(strerror(errno)));
		}

		// if there is something we set it, this avoids problems with FreeBSD 5.x
		if (_text_acl_default.size() > 0)
		{
			acl_t acl_default = acl_from_text(_text_acl_default.c_str());
			if (acl_access == NULL)
			{
				std::cerr << "Default ACL is wrong!!!" << endl << _text_acl_default.c_str() << endl;
				throw ACLManagerException(_("Default textual representation of the ACL is wrong"));
			}

			if (acl_set_file(_filename.c_str(), ACL_TYPE_DEFAULT, acl_default) != 0)
			{
				throw ACLManagerException(Glib::locale_to_utf8(strerror(errno)));
			}
		}
	}
	acl_free(acl_access);
}


void ACLManager::clear_default_acl()
{
	_there_is_default_user = _there_is_default_group = _there_is_default_others = _there_is_default_mask = false;
	_default_user_acl.clear();	
	_default_group_acl.clear();	
	update_changes_acl_default();
}

void ACLManager::clear_all_acl()
{
	_user_acl.clear();
	_group_acl.clear();
	_there_is_mask = false;
	_there_is_default_user = false;
	_there_is_default_group = false;
	_there_is_default_others = false;
	_there_is_default_mask = false;
	create_textual_representation();
	commit_changes_to_file();
}

void ACLManager::calculate_access_mask()
{
	// De moment afegirem una màscara laxa
	_there_is_mask = true;
	_mask_acl = permissions_t(7);
	create_textual_representation();
}

void ACLManager::modify_owner_perms(permissions_t& p)
{
	_owner_perms = p;
	update_changes_acl_access();
}

void ACLManager::modify_group_perms(permissions_t& p)
{
	_group_perms = p;
	update_changes_acl_access();
}

void ACLManager::modify_others_perms(permissions_t& p)
{
	_others_perms = p;
	update_changes_acl_access();
}

void ACLManager::modify_mask(permissions_t& p)
{
	_there_is_mask = true;
	_mask_acl = p;
	update_changes_acl_access();
}

void ACLManager::modify_owner_perms_default(permissions_t& p)
{
	_there_is_default_user = true;
	_default_user = p;
	fill_needed_acl_default();
	update_changes_acl_default();
}

void ACLManager::modify_group_perms_default(permissions_t& p)
{
	_there_is_default_group = true;
	_default_group = p;
	fill_needed_acl_default();
	update_changes_acl_default();
}

void ACLManager::modify_others_perms_default(permissions_t& p)
{
	_there_is_default_others = true;
	_default_others = p;
	fill_needed_acl_default();
	update_changes_acl_default();
}

void ACLManager::modify_mask_default(permissions_t& p)
{
	_there_is_default_mask = true;
	_default_mask = p;
	fill_needed_acl_default();
	update_changes_acl_default();
}

void ACLManager::create_default_acl()
{
	fill_needed_acl_default();
	update_changes_acl_default();
}
