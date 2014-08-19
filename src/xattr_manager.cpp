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
#include "xattr_manager.hpp"

XAttrManager::XAttrManager(const Glib::ustring& filename) throw (XAttrManagerException)
    : _filename(filename)
{
    // Check it is an ordinary file or a directory
    struct stat buffer;
    if (stat(_filename.c_str(), &buffer) == -1)
    {
        throw XAttrManagerException(Glib::locale_to_utf8(strerror(errno)));
    }

    if (!S_ISREG(buffer.st_mode) && !S_ISDIR(buffer.st_mode))
    {
        throw XAttrManagerException(_("Only regular files or directories supported")); 
    }

    this->_owner = buffer.st_uid;
    
    // FIXME Crappy way to detect if we can modify xattrs,
    // it will thrown a exception if they cannot be read
    read_test();
}

// We should find better ways to test xattr support
void XAttrManager::read_test() throw (XAttrManagerException)
{
    Glib::ustring qualified_attr_name = "user.test";
    int buffer_length;
    int size = 30;
    char* buffer = new char[size];

    buffer_length = getxattr (_filename.c_str(), qualified_attr_name.c_str(),
            buffer, size);

    if (buffer_length == -1 && 
            errno != ENOATTR && 
            errno != ERANGE)
    {
        delete[] buffer;
        throw XAttrManagerException(Glib::locale_to_utf8(strerror(errno)));
    }

    delete[] buffer;
}

std::vector<std::string> XAttrManager::get_xattr_list() throw (XAttrManagerException)
{
    std::vector<std::string> result;

    int size = listxattr(_filename.c_str(), NULL, 0);

    // Assume at first that the length of every element will be at most 30 chars
    size = size*30;
    char* buffer = new char[size];

    int num_elems;
    num_elems = listxattr(_filename.c_str(), buffer, size);

    while ((num_elems == -1) && (errno == ERANGE))
    {
        delete[] buffer;
        size = size*2;
        buffer = new char[size];
        num_elems = listxattr(_filename.c_str(), buffer, size);
    }

    // num_elems == -1 && errno != ERANGE
    if (num_elems == -1)
    {
        delete[] buffer;
        throw XAttrManagerException(Glib::locale_to_utf8(strerror(errno)));
    }

    for (int begin = 0, current = 0; current < num_elems; current++)
    {
        if (buffer[current] == '\0')
        {
            // Get the value
            std::string attr_name(&buffer[begin]);

            // Some filesystems give too small results
            if (attr_name.size() > 5)
            {
                std::string prefix =  attr_name.substr(0, 5);
                std::string postfix = attr_name.substr(5);

                // Check the prefix, we only want user_xattr
                if (prefix == "user.")
                {
                    bool is_empty = false;

                    try
                    {
                        std::string attribute_value = get_attribute_value(postfix);
                    }
                    catch (XAttrManagerException e)
                    {
                        is_empty = true;
                    }

                    if (!is_empty)
                    {
                        result.push_back(postfix);
                    }
                }
            }
            
            begin = current + 1;
        }
    }
    
    delete[] buffer;

    return result;
}

std::string XAttrManager::get_attribute_value(const std::string& attr_name) throw (XAttrManagerException)
{
    int size = 30;
    char* buffer = new char[size];

    std::string qualified_attr_name = "user." + attr_name;

    int length_buffer = getxattr (_filename.c_str(), qualified_attr_name.c_str(),
            buffer, size);

    while ((length_buffer == -1) && (errno == ERANGE))
    {
        delete[] buffer;
        size = size*2;
        buffer = new char[size];

        length_buffer = getxattr (_filename.c_str(), qualified_attr_name.c_str(),
                buffer, size);
    }

    if (length_buffer == -1)
    {
        delete[] buffer;
        throw XAttrManagerException(Glib::locale_to_utf8(strerror(errno)));
    }

    char* new_buffer = new char[length_buffer + 1];
    new_buffer[length_buffer] = '\0';

    for (int i = 0; i < length_buffer; i++)
    {
        new_buffer[i] = buffer[i];
    }

    std::string attr_value(new_buffer);

    delete[] new_buffer;
    delete[] buffer;

    return attr_value;
}

XAttrManager::attributes_t XAttrManager::get_attributes_list()
{
    std::vector<std::string> attributes;
    std::vector<std::string>::iterator it;

    attributes = get_xattr_list();

    attributes_t result;

    for (it = attributes.begin(); it != attributes.end(); it++)
    {
        std::string attr_value = get_attribute_value(*it);

        result[*it] = attr_value;
    }

    return result;
}

void XAttrManager::remove_attribute(std::string attr_name)
{
    std::string qualified_name = "user." + attr_name;
    int result = removexattr (_filename.c_str(), qualified_name.c_str());

    if (result != 0)
    {
        throw XAttrManagerException(Glib::locale_to_utf8(strerror(errno)));
    }
}

void XAttrManager::add_attribute(std::string attr_name, std::string attr_value)
{
    std::string qualified_attr_name = "user." + attr_name;
    int result = setxattr (_filename.c_str(), qualified_attr_name.c_str(),
            attr_value.c_str(), attr_value.size(), 0);

    if (result != 0)
    {
        throw XAttrManagerException(Glib::locale_to_utf8(strerror(errno)));
    }
}

void XAttrManager::change_attribute_name(std::string old_attr_name, std::string new_attr_name)
{
    std::string attribute_value = get_attribute_value(old_attr_name);
    add_attribute(new_attr_name, attribute_value);
    remove_attribute(old_attr_name);
}
