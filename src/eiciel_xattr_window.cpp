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

#include "eiciel_xattr_window.hpp"

EicielXAttrWindow::EicielXAttrWindow(EicielXAttrController* contr)
    : _controller(contr), 
    _b_add_attribute(Gtk::Stock::ADD),
    _b_remove_attribute(Gtk::Stock::REMOVE)
{
    _ref_xattr_list = Gtk::ListStore::create(_xattr_list_model);

    _controller->_window = this;

    set_border_width(4);

    _ref_xattr_list->set_sort_column(_xattr_list_model._attribute_name, Gtk::SORT_ASCENDING);

    _xattr_listview.set_reallocate_redraws();
    _xattr_listview.set_model(_ref_xattr_list);

    // Add columns and link them to the model
    _xattr_listview.append_column(_("Name"), _xattr_list_model._attribute_name);

    // This column has special requirements
    Gtk::TreeViewColumn* col = _xattr_listview.get_column(0);
#ifdef USING_GNOME2
    Gtk::CellRenderer* cellRenderer = col->get_first_cell_renderer();
#else
    Gtk::CellRenderer* cellRenderer = col->get_first_cell();
#endif
    Gtk::CellRendererText* cellRendererText = dynamic_cast<Gtk::CellRendererText*>(cellRenderer);
    cellRendererText->property_editable() = true;

    cellRendererText->signal_edited().connect(
            sigc::mem_fun(*this, &EicielXAttrWindow::set_name_edited_attribute)
            );

    _xattr_listview.append_column_editable(_("Value"), _xattr_list_model._attribute_value);

    col = _xattr_listview.get_column(1);
#ifdef USING_GNOME2
    cellRenderer = col->get_first_cell_renderer();
#else
    cellRenderer = col->get_first_cell();
#endif
    cellRendererText = dynamic_cast<Gtk::CellRendererText*>(cellRenderer);
    cellRendererText->property_editable() = true;

    cellRendererText->signal_edited().connect(
            sigc::mem_fun(*this, &EicielXAttrWindow::set_value_edited_attribute)
            );

    _xattr_listview_container.set_size_request(-1, 100);
    _xattr_listview_container.add(_xattr_listview);
    _xattr_listview_container.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    pack_start(_xattr_listview_container, Gtk::PACK_EXPAND_WIDGET, 2);

    _bottom_buttonbox.pack_start(_b_add_attribute, Gtk::PACK_SHRINK, 2);
    _bottom_buttonbox.pack_start(_b_remove_attribute, Gtk::PACK_SHRINK, 2);

    pack_start(_bottom_buttonbox, Gtk::PACK_SHRINK, 2);

    // Signal binding
    Glib::RefPtr<Gtk::TreeSelection> referenciaSeleccioLlistaXAttr = _xattr_listview.get_selection();

    // Selection change in XAttr
    referenciaSeleccioLlistaXAttr->signal_changed().connect(
            sigc::mem_fun(*this, &EicielXAttrWindow::_xattr_selection_change)
            );

    // Remove button
    _b_remove_attribute.signal_clicked().connect(
            sigc::mem_fun(*this, &EicielXAttrWindow::remove_selected_attribute)
            );
    
    // Edit button
    _b_add_attribute.signal_clicked().connect(
            sigc::mem_fun(*this, &EicielXAttrWindow::add_selected_attribute)
            );

    show_all();

    set_active(false);
}

void EicielXAttrWindow::set_active(bool b)
{
    this->set_sensitive(b);
}

EicielXAttrWindow::~EicielXAttrWindow()
{
    delete _controller;
}

void EicielXAttrWindow::_xattr_selection_change()
{
    Glib::RefPtr<Gtk::TreeSelection> referenciaSeleccioLlista = _xattr_listview.get_selection();
    Gtk::TreeModel::iterator iter = referenciaSeleccioLlista->get_selected();
    if (!iter)
    {
        there_is_no_xattr_selection();
    }
    else
    {
        there_is_xattr_selection();
    }
}

void EicielXAttrWindow::there_is_no_xattr_selection()
{
    _b_remove_attribute.set_sensitive(false);
}

void EicielXAttrWindow::there_is_xattr_selection()
{
    if (!this->_readonly)
    {
        _b_remove_attribute.set_sensitive(true);
    }
}

void EicielXAttrWindow::set_name_edited_attribute(const Glib::ustring& path, const Glib::ustring& value)
{
    Gtk::TreeModel::iterator iter = _ref_xattr_list->get_iter(path);

    if (iter)
    {
        Gtk::TreeModel::Row row(*iter);

        // Can't be empty
        if (value.empty())
        {
            return;
        }

        // We do not allow repeated
        Gtk::TreeModel::Children children = _ref_xattr_list->children();

        for(Gtk::TreeModel::Children::iterator iter = children.begin(); 
                iter != children.end(); ++iter)
        {
            Gtk::TreeModel::Row irow(*iter);

            if (irow[_xattr_list_model._attribute_name] == value)
            {
                return;
            }
        }

        try
        {
            _controller->update_attribute_name(row[_xattr_list_model._attribute_name], value);
            row[_xattr_list_model._attribute_name] = value;
        }
        catch (XAttrManagerException e)
        {
            Glib::ustring s = _("Could not rename attribute name: ") + e.getMessage();
            Gtk::Container* toplevel = this->get_toplevel();
            if (toplevel == NULL
                    || !toplevel->get_is_toplevel())
            {
                Gtk::MessageDialog renameXAttr(s, false,
                        Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
                renameXAttr.run();
            }
            else
            {
                Gtk::MessageDialog renameXAttr(
                        *(Gtk::Window*)toplevel,
                        s, false,
                        Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
                renameXAttr.run();
            }
        }
    }
}

void EicielXAttrWindow::set_value_edited_attribute(const Glib::ustring& path, const Glib::ustring& value)
{
    Gtk::TreeModel::iterator iter = _ref_xattr_list->get_iter(path);

    if (iter)
    {
        Gtk::TreeModel::Row row(*iter);
        try
        {
            _controller->update_attribute_value(row[_xattr_list_model._attribute_name], value);
            row[_xattr_list_model._attribute_value] = value;
        }
        catch (XAttrManagerException e)
        {
            Glib::ustring s = _("Could not change attribute value: ") + e.getMessage();
            Gtk::Container* toplevel = this->get_toplevel();
            if (toplevel == NULL
                    || !toplevel->get_is_toplevel())
            {
                Gtk::MessageDialog editXAttr(s, false,
                        Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
                editXAttr.run();
            }
            else
            {
                Gtk::MessageDialog editXAttr(
                        *(Gtk::Window*)toplevel,
                        s, false,
                        Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
                editXAttr.run();
            }
        }
    }
}

void EicielXAttrWindow::remove_selected_attribute()
{
    Glib::RefPtr<Gtk::TreeSelection> referenciaSeleccioLlista = _xattr_listview.get_selection();
    Gtk::TreeModel::iterator iter = referenciaSeleccioLlista->get_selected();

    if (iter)
    {
        Gtk::TreeModel::Row row(*iter);

        try
        {
            _controller->remove_attribute(row[_xattr_list_model._attribute_name]);
            _ref_xattr_list->erase(iter);
        }
        catch (XAttrManagerException e)
        {
            Glib::ustring s = _("Could not remove attribute: ") + e.getMessage();
            Gtk::Container* toplevel = this->get_toplevel();
            if (toplevel == NULL
                    || !toplevel->get_is_toplevel())
            {
                Gtk::MessageDialog removeXAttr(s, false,
                        Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
                removeXAttr.run();
            }
            else
            {
                Gtk::MessageDialog removeXAttr(
                        *(Gtk::Window*)toplevel,
                        s, false,
                        Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
                removeXAttr.run();
            }
        }
    }
}

void EicielXAttrWindow::add_selected_attribute()
{
    // Compute the name of the new attribute
    bool repeated;
    int num_times = 0;

    Glib::ustring new_name;

    do {
        if (num_times == 0)
        {
            new_name = _("New attribute");
        }
        else
        {
            char* num_timesStr = new char[20];

            snprintf(num_timesStr, 20, " (%d)", num_times);
            num_timesStr[19] = '\0';

            new_name = _("New attribute");
            new_name += num_timesStr;

            delete[] num_timesStr;
        }
        num_times++;

        Gtk::TreeModel::Children children = _ref_xattr_list->children();

        repeated = false;
        for(Gtk::TreeModel::Children::iterator iter = children.begin(); 
                iter != children.end(); ++iter)
        {
            Gtk::TreeModel::Row irow(*iter);

            if (irow[_xattr_list_model._attribute_name] == new_name)
            {
                repeated = true;
                break;
            }
        }
    }
    while (repeated);
    
    Gtk::TreeModel::iterator iter = _ref_xattr_list->append();
    Gtk::TreeModel::Row row;
    row = *iter;

    row[_xattr_list_model._attribute_name] = new_name;
    row[_xattr_list_model._attribute_value] = _("New value");

    try
    {
        _controller->add_attribute(row[_xattr_list_model._attribute_name],
                row[_xattr_list_model._attribute_value]);

        Gtk::TreePath path = _ref_xattr_list->get_path(iter);
        Gtk::TreeViewColumn* col = _xattr_listview.get_column(0);

        _xattr_listview.set_cursor(path, *col, true);
    }
    catch (XAttrManagerException e)
    {
        _ref_xattr_list->erase(iter);
        Glib::ustring s = _("Could not add attribute: ") + e.getMessage();
        Gtk::Container* toplevel = this->get_toplevel();
        if (toplevel == NULL
                || !toplevel->get_is_toplevel())
        {
            Gtk::MessageDialog addXAttr_message(s, false,
                    Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
            addXAttr_message.run();
        }
        else
        {
            Gtk::MessageDialog addXAttr_message(
                    *(Gtk::Window*)toplevel,
                    s, false,
                    Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
            addXAttr_message.run();
        }
    }
}

void EicielXAttrWindow::fill_attributes(XAttrManager::attributes_t llista)
{
    XAttrManager::attributes_t::iterator it;
    _ref_xattr_list->clear();

    Gtk::TreeModel::iterator iter;
    Gtk::TreeModel::Row row;
    
    for (it = llista.begin(); it != llista.end(); it++)
    {
        iter = _ref_xattr_list->append();
        row = *iter;
        
        row[_xattr_list_model._attribute_name] = it->first;
        row[_xattr_list_model._attribute_value] = it->second;
    }
}

void EicielXAttrWindow::set_readonly(bool b)
{
    _readonly = b;

    _b_add_attribute.set_sensitive(!b);
    _b_remove_attribute.set_sensitive(!b);

    Gtk::TreeViewColumn* col = _xattr_listview.get_column(0);
#ifdef USING_GNOME2
    Gtk::CellRenderer* cellRenderer = col->get_first_cell_renderer();
#else
    Gtk::CellRenderer* cellRenderer = col->get_first_cell();
#endif
    Gtk::CellRendererText* cellRendererText = dynamic_cast<Gtk::CellRendererText*>(cellRenderer);
    cellRendererText->property_editable() = !b;

    col = _xattr_listview.get_column(1);
#ifdef USING_GNOME2
    cellRenderer = col->get_first_cell_renderer();
#else
    cellRenderer = col->get_first_cell();
#endif
    cellRendererText = dynamic_cast<Gtk::CellRendererText*>(cellRenderer);
    cellRendererText->property_editable() = !b;
}
