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
#ifndef GESTOR_ACL_HPP
#define GESTOR_ACL_HPP

#include <config.hpp>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <glib/gi18n-lib.h>
#include <glibmm.h>
#include <sys/acl.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef HAVE_ACL_LIBACL_H
#include <acl/libacl.h>
#endif
#include <algorithm>
#include <errno.h>
#include <grp.h>
#include <iostream>
#include <pwd.h>
#include <sstream>
#include <unistd.h>
#include <vector>

#ifdef HAVE_ACL_GET_PERM
#define ACL_GET_PERM acl_get_perm
#else
#ifdef HAVE_ACL_GET_PERM_NP
#define ACL_GET_PERM acl_get_perm_np
#else
#error "An acl_get_perm-like function is needed"
#endif
#endif

class ACLManagerException {
public:
    ACLManagerException(Glib::ustring missatge)
        : _missatge(missatge)
    {
    }

    Glib::ustring getMessage() const { return _missatge; }

private:
    Glib::ustring _missatge;
};

struct permissions_t {
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
        : reading(rd)
        , writing(wr)
        , execution(ex)
    {
    }
    permissions_t()
        : reading(false)
        , writing(false)
        , execution(false)
    {
    }
};

struct acl_entry : permissions_t {
    int qualifier; // Group or user
    std::string name; // Symbolic name of the qualifier
    bool valid_name;
};

class ACLManager {
private:
    std::string _filename;
    bool _is_directory;

    uid_t _uid_owner;
    std::string _owner_name;
    permissions_t _owner_perms;

    std::string _group_name;
    permissions_t _group_perms;

    permissions_t _others_perms;

    bool _there_is_mask;
    permissions_t _mask_acl;

    std::vector<acl_entry> _user_acl;
    std::vector<acl_entry> _group_acl;
    std::vector<acl_entry> _default_user_acl;
    std::vector<acl_entry> _default_group_acl;

    permissions_t _default_user;
    bool _there_is_default_user;

    permissions_t _default_group;
    bool _there_is_default_group;

    permissions_t _default_others;
    bool _there_is_default_others;

    permissions_t _default_mask;
    bool _there_is_default_mask;

    std::string _text_acl_access;
    std::string _text_acl_default;

    void get_ugo_permissions();
    void get_acl_entries_access();
    void get_acl_entries_default();
    void create_textual_representation();
    std::string permission_to_str(permissions_t& p);
    std::string write_name(acl_entry& eacl);
    void set_acl_generic(const std::string& nom,
        std::vector<acl_entry>& llistACL,
        const permissions_t& perms);
    void remove_acl_generic(const std::string& nom,
        std::vector<acl_entry>& llistaACL);

    void commit_changes_to_file();
    void calculate_access_mask();
    void update_changes_acl_access();
    void update_changes_acl_default();
    void fill_needed_acl_default();

    class ACLEquivalence {
    private:
        std::string _qualifier;

    public:
        ACLEquivalence(const std::string& qualif)
            : _qualifier(qualif)
        {
        }
        bool operator()(acl_entry& a)
        {
            return (a.valid_name && (a.name == _qualifier));
        }
    };

public:
    const static int PERM_READ = 0;
    const static int PERM_WRITE = 1;
    const static int PERM_EXEC = 2;

    ACLManager(const std::string& filename) throw(ACLManagerException);

    std::string get_acl_access() const { return _text_acl_access; }
    std::string get_acl_default() const { return _text_acl_default; }

    bool is_directory() const { return _is_directory; }

    void modify_acl_user(const std::string& username, const permissions_t& perms);
    void modify_acl_group(const std::string& groupname,
        const permissions_t& perms);

    void modify_acl_default_user(const std::string& username,
        const permissions_t& perms);
    void modify_acl_default_group(const std::string& groupname,
        const permissions_t& perms);

    void modify_owner_perms(permissions_t& p);
    void modify_group_perms(permissions_t& p);
    void modify_others_perms(permissions_t& p);
    void modify_mask(permissions_t& p);

    void modify_owner_perms_default(permissions_t& p);
    void modify_group_perms_default(permissions_t& p);
    void modify_others_perms_default(permissions_t& p);
    void modify_mask_default(permissions_t& p);

    void remove_acl_user(const std::string& username);
    void remove_acl_group(const std::string& groupname);
    void remove_acl_user_default(const std::string& username);
    void remove_acl_group_default(const std::string& groupname);

    void clear_all_acl();
    void clear_default_acl();

    void create_default_acl();

    std::vector<acl_entry> get_acl_user() const { return _user_acl; }
    std::vector<acl_entry> get_acl_group() const { return _group_acl; }
    std::vector<acl_entry> get_acl_user_default() const
    {
        return _default_user_acl;
    }
    std::vector<acl_entry> get_acl_group_default() const
    {
        return _default_group_acl;
    }
    permissions_t get_mask() { return _mask_acl; }

    permissions_t get_user() { return _owner_perms; }
    permissions_t get_group() { return _group_perms; }
    permissions_t get_other() { return _others_perms; }

    permissions_t get_user_default() { return _default_user; }
    permissions_t get_group_default() { return _default_group; }
    permissions_t get_other_default() { return _default_others; }
    permissions_t get_mask_default() { return _default_mask; }

    std::string get_owner_name() { return _owner_name; }
    std::string get_group_name() { return _group_name; }
    uid_t get_owner_uid() { return _uid_owner; }

    bool has_mask() const { return _there_is_mask; }
    bool has_default_user() const { return _there_is_default_user; }
    bool has_default_group() const { return _there_is_default_group; }
    bool has_default_other() const { return _there_is_default_others; }
    bool has_default_mask() const { return _there_is_default_mask; }
};

#endif
