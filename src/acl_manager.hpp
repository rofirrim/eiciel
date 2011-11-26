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
#ifndef GESTOR_ACL_HPP
#define GESTOR_ACL_HPP

#include <config.hpp>

#include <cstring>
#include <string>
#include <vector>
#include <iostream>

#include <glibmm.h>
#include <glib/gi18n-lib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/acl.h>
#ifdef HAVE_ACL_LIBACL_H
 #include <acl/libacl.h>
#endif
#include <unistd.h>
#include <errno.h>
#include <vector>
#include <iostream>
#include <pwd.h>
#include <grp.h>
#include <sstream>
#include <algorithm>

using namespace std;

#ifdef HAVE_ACL_GET_PERM
   #define ACL_GET_PERM acl_get_perm
#else 
 #ifdef HAVE_ACL_GET_PERM_NP
   #define ACL_GET_PERM acl_get_perm_np 
 #else
   #error "An acl_get_perm-like function is needed" 
 #endif
#endif
  


class ACLManagerException
{
	public:
		ACLManagerException(Glib::ustring missatge) : _missatge(missatge) 
		{
		}

		Glib::ustring getMessage() const
		{ 
			return _missatge; 
		}
	private:
		Glib::ustring _missatge;
};

struct permissions_t 
{
	bool reading;
	bool writing;
	bool execution;

    // Convenience constructors
	permissions_t(char c)
	{
		reading = (c & 04);
		writing = (c & 02);
		execution = (c & 01);
	}
	permissions_t(bool rd, bool wr, bool ex)
		: reading(rd), writing(wr), execution(ex) {}
	permissions_t() 
        : reading(false), writing(false), execution(false) {}
};


struct acl_entry : permissions_t
{
	int qualifier; // Group or user
	string name; // Symbolic name of the qualifier
	bool valid_name;
};

class ACLManager
{
	private:
		string _filename;
		bool _is_directory;

		uid_t _uid_owner;
		string _owner_name;
		permissions_t _owner_perms;

		string _group_name;
		permissions_t _group_perms;

		permissions_t _others_perms;

		bool _there_is_mask;
		permissions_t _mask_acl;

		vector<acl_entry> _user_acl;
		vector<acl_entry> _group_acl;
		vector<acl_entry> _default_user_acl;
		vector<acl_entry> _default_group_acl;

		permissions_t _default_user; 
		bool _there_is_default_user;

		permissions_t _default_group;
		bool _there_is_default_group;

		permissions_t _default_others;
		bool _there_is_default_others;

		permissions_t _default_mask;
		bool _there_is_default_mask;

		string _text_acl_access;
		string _text_acl_default;

		void get_ugo_permissions();
		void get_acl_entries_access();
		void get_acl_entries_default();
		void create_textual_representation();
		string permission_to_str(permissions_t& p);
		string write_name(acl_entry& eacl);
		void set_acl_generic(const string& nom, vector<acl_entry>& llistACL, const permissions_t& perms);
		void remove_acl_generic(const string& nom, vector<acl_entry>& llistaACL);

		void commit_changes_to_file();
		void calculate_access_mask();
		void update_changes_acl_access();
		void update_changes_acl_default();
		void fill_needed_acl_default();

		class ACLEquivalence
		{
			private:
				string _qualifier;
			public:
				ACLEquivalence(const string& qualif)
					: _qualifier(qualif) {}
				bool operator ()(acl_entry& a)
				{
					return (a.valid_name && (a.name == _qualifier));
				}
		};
	public:
		const static int PERM_READ = 0;
		const static int PERM_WRITE = 1;
		const static int PERM_EXEC = 2;

		ACLManager(const string& filename) throw (ACLManagerException);
		
		string get_acl_access() const { return _text_acl_access; }
		string get_acl_default() const { return _text_acl_default; }
		
		bool is_directory() const { return _is_directory; }
		
		void modify_acl_user(const string& username, const permissions_t& perms);
		void modify_acl_group(const string& groupname, const permissions_t& perms);
		
		void modify_acl_default_user(const string& username, const permissions_t& perms);
		void modify_acl_default_group(const string& groupname, const permissions_t& perms);

		void modify_owner_perms(permissions_t& p);
		void modify_group_perms(permissions_t& p);
		void modify_others_perms(permissions_t& p);
		void modify_mask(permissions_t& p);

		void modify_owner_perms_default(permissions_t& p);
		void modify_group_perms_default(permissions_t& p);
		void modify_others_perms_default(permissions_t& p);
		void modify_mask_default(permissions_t& p);
		
		void remove_acl_user(const string& username);
		void remove_acl_group(const string& groupname);
		void remove_acl_user_default(const string& username);
		void remove_acl_group_default(const string& groupname);

		void clear_all_acl();
		void clear_default_acl();

		void create_default_acl();

		vector<acl_entry> get_acl_user() const { return _user_acl; }
		vector<acl_entry> get_acl_group() const { return _group_acl; }
		vector<acl_entry> get_acl_user_default() const { return _default_user_acl; }
		vector<acl_entry> get_acl_group_default() const { return _default_group_acl; }
		permissions_t get_mask() { return _mask_acl; }

		permissions_t get_user() { return _owner_perms; }
		permissions_t get_group() { return _group_perms; }
		permissions_t get_other() { return _others_perms; }
		
		permissions_t get_user_default() { return _default_user; }
		permissions_t get_group_default() { return _default_group; }
		permissions_t get_other_default() { return _default_others; }
		permissions_t get_mask_default() { return _default_mask; }

		string get_owner_name() { return _owner_name; }
		string get_group_name() { return _group_name; }
		uid_t get_owner_uid() { return _uid_owner; }

		bool has_mask() const { return _there_is_mask; }
		bool has_default_user() const { return _there_is_default_user; }
		bool has_default_group() const { return _there_is_default_group; }
		bool has_default_other() const { return _there_is_default_others; }
		bool has_default_mask() const { return _there_is_default_mask; }
};

#endif
