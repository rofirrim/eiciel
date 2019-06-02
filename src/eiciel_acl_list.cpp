/*
   Eiciel - GNOME editor of ACL file permissions.
   Copyright (C) 2019 Roger Ferrer Ibáñez

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

#include "eiciel_acl_list.hpp"
#include "eiciel_acl_list_controller.hpp"

// Constructor
EicielACLList::EicielACLList(EicielACLListController* cont)
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
    /* GUI */
    , _main_box(Gtk::ORIENTATION_VERTICAL)
    , _listview_acl_container()
    , _listview_acl()
    , _warning_hbox()
    , _warning_icon(Gtk::Stock::DIALOG_WARNING,
          Gtk::IconSize(Gtk::ICON_SIZE_SMALL_TOOLBAR))
    , _warning_label(_("There are ineffective permissions"))
    , _middle_button_group()
    , _b_remove_acl(_("Remove participant from ACL"))
    , _cb_modify_default_acl(_("Edit default participants"))
    /* Non GUI */
    , _readonly_mode(false)
    , _toggling_default_acl(false)
    , _controller(cont) // Assign _controller to _window
{
    // Let know the controller we are its view.
    _controller->set_view(this);

    this->set_margin_top(12);
    this->set_margin_bottom(12);
    this->set_margin_start(12);
    this->set_margin_end(12);

    this->pack_start(_main_box);

    _main_box.set_spacing(4);

#ifndef USING_GNOME2
    _middle_button_group.set_spacing(2);
#endif

    // Open icons
    _user_icon = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/user.png");
    _user_icon_acl = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/user-acl.png");
    _group_icon = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/group.png");
    _group_icon_acl = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/group-acl.png");
    _others_icon = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/others.png");
    _mask_icon = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/mask.png");

    _default_user_icon = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/user-default.png");
    _default_group_icon = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/group-default.png");
    _default_others_icon = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/others-default.png");
    _default_user_icon_acl = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/user-acl-default.png");
    _default_group_icon_acl = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/group-acl-default.png");
    _default_mask_icon = Gdk::Pixbuf::create_from_file(PKGDATADIR "/img/mask-default.png");

    // ACL list
    _ref_acl_list = create_acl_list_store();

    // Set columns in the ACL list
    _listview_acl.set_model(_ref_acl_list);
    _listview_acl.append_column("", _acl_list_model._icon);
    _listview_acl.append_column(_("Entry"), _acl_list_model._entry_name);

    CellRendererACL* renderRead = Gtk::manage(new CellRendererACL());
    int numColumns = _listview_acl.append_column(_("Read"), *renderRead);
    Gtk::TreeViewColumn* reading_column = _listview_acl.get_column(numColumns - 1);

    if (reading_column != NULL) {
        reading_column->add_attribute(renderRead->property_active(),
            _acl_list_model._reading_permission);
        reading_column->add_attribute(renderRead->mark_background(),
            _acl_list_model._reading_ineffective);
        reading_column->set_cell_data_func(
            *renderRead, mem_fun(*this, &EicielACLList::acl_cell_data_func));
    }

    CellRendererACL* renderWrite = Gtk::manage(new CellRendererACL());
    numColumns = _listview_acl.append_column(_("Write"), *renderWrite);
    Gtk::TreeViewColumn* writing_column = _listview_acl.get_column(numColumns - 1);
    if (writing_column != NULL) {
        writing_column->add_attribute(renderWrite->property_active(),
            _acl_list_model._writing_permission);
        writing_column->add_attribute(renderWrite->mark_background(),
            _acl_list_model._writing_ineffective);
        writing_column->set_cell_data_func(
            *renderWrite, mem_fun(*this, &EicielACLList::acl_cell_data_func));
    }

    CellRendererACL* renderExecute = Gtk::manage(new CellRendererACL());
    numColumns = _listview_acl.append_column(_("Execute"), *renderExecute);
    Gtk::TreeViewColumn* execution_column = _listview_acl.get_column(numColumns - 1);
    if (execution_column != NULL) {
        execution_column->add_attribute(renderExecute->property_active(),
            _acl_list_model._execution_permission);
        execution_column->add_attribute(renderExecute->mark_background(),
            _acl_list_model._execution_ineffective);
        execution_column->set_cell_data_func(
            *renderExecute, mem_fun(*this, &EicielACLList::acl_cell_data_func));
    }

    // Aesthetic column completely useless
    _listview_acl.append_column("", _acl_list_model._empty);

    _listview_acl_container.add(_listview_acl);
    _listview_acl_container.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

    _listview_acl_container.set_size_request(-1, 150);
    _listview_acl_container.set_shadow_type(Gtk::SHADOW_IN);

    _main_box.pack_start(_listview_acl_container);

    _warning_hbox.set_spacing(4);
    _warning_icon.set_halign(Gtk::ALIGN_START);
    _warning_hbox.pack_start(_warning_icon, Gtk::PACK_SHRINK);
    _warning_label.set_halign(Gtk::ALIGN_START);
    _warning_hbox.pack_start(_warning_label, Gtk::PACK_SHRINK);

    _main_box.pack_start(_warning_hbox, Gtk::PACK_SHRINK);

    // Add 'move up' 'move down' buttons
    _middle_button_group.pack_end(_b_remove_acl, Gtk::PACK_SHRINK);
    _middle_button_group.pack_end(_cb_modify_default_acl, Gtk::PACK_SHRINK);

    _main_box.pack_start(_middle_button_group, Gtk::PACK_SHRINK);

    // Signal binding
    Glib::RefPtr<Gtk::TreeSelection> acl_list_selection_reference = _listview_acl.get_selection();

    // Filter function this time only for _readonly_mode
    acl_list_selection_reference->set_select_function(
        sigc::mem_fun(*this, &EicielACLList::acl_selection_function));

    // ACL selection change event
    acl_list_selection_reference->signal_changed().connect(
        sigc::mem_fun(*this, &EicielACLList::acl_selection_change));

    // Remove ACL button
    _b_remove_acl.signal_clicked().connect(
        sigc::mem_fun(*this, &EicielACLList::remove_selected_acl));

    // Change of default acl toggle button
    _cb_modify_default_acl.signal_clicked().connect(
        sigc::mem_fun(*this, &EicielACLList::toggle_edit_default_acl));

    // Column edition event
    Gtk::TreeViewColumn* col = _listview_acl.get_column(2);
#ifdef USING_GNOME2
    Gtk::CellRenderer* render = col->get_first_cell_renderer(); // Only one
#else
    Gtk::CellRenderer* render = col->get_first_cell(); // Only one
#endif
    CellRendererACL* toggle_render = dynamic_cast<CellRendererACL*>(render);

    toggle_render->signal_toggled().connect(sigc::bind<PermissionKind>(
        sigc::mem_fun(*this, &EicielACLList::change_permissions), PK_READING));

    col = _listview_acl.get_column(3);
#ifdef USING_GNOME2
    render = col->get_first_cell_renderer();
#else
    render = col->get_first_cell();
#endif
    toggle_render = dynamic_cast<CellRendererACL*>(render);

    toggle_render->signal_toggled().connect(sigc::bind<PermissionKind>(
        sigc::mem_fun(*this, &EicielACLList::change_permissions), PK_WRITING));

    col = _listview_acl.get_column(4);
#ifdef USING_GNOME2
    render = col->get_first_cell_renderer();
#else
    render = col->get_first_cell();
#endif
    toggle_render = dynamic_cast<CellRendererACL*>(render);

    toggle_render->signal_toggled().connect(sigc::bind<PermissionKind>(
        sigc::mem_fun(*this, &EicielACLList::change_permissions), PK_EXECUTION));

    show_all();
}

Glib::RefPtr<Gtk::ListStore> EicielACLList::create_acl_list_store()
{
    return Gtk::ListStore::create(_acl_list_model);
}

void EicielACLList::set_exist_ineffective_permissions(bool b)
{
    if (b) {
        _warning_label.show();
        _warning_icon.show();
    } else {
        _warning_icon.hide();
        _warning_label.hide();
    }
}

EicielACLList::~EicielACLList()
{
}

void EicielACLList::set_active(bool b)
{
    _main_box.set_sensitive(b);
}

void EicielACLList::set_readonly(bool b)
{
    _readonly_mode = b;
    if (b) {
        _cb_modify_default_acl.set_sensitive(false);
    }
}

bool EicielACLList::acl_selection_function(
    const Glib::RefPtr<Gtk::TreeModel>& model,
    const Gtk::TreeModel::Path& path,
    bool)
{
    return !_readonly_mode;
}

void EicielACLList::empty_acl_list()
{
    _ref_acl_list->clear();
}

void EicielACLList::replace_acl_store(Glib::RefPtr<Gtk::ListStore> ref_acl_list)
{
    _listview_acl.set_model(ref_acl_list);
    _ref_acl_list = ref_acl_list;
}

Glib::RefPtr<Gtk::ListStore> EicielACLList::get_acl_store()
{
    return _ref_acl_list;
}

void EicielACLList::acl_selection_change()
{
    Glib::RefPtr<Gtk::TreeSelection> selection_list_reference = _listview_acl.get_selection();
    Gtk::TreeModel::iterator iter = selection_list_reference->get_selected();
    if (!iter || _readonly_mode) {
        there_is_no_acl_selection();
    } else {
        if (!(*iter)[_acl_list_model._removable])
            there_is_no_acl_selection();
        else
            there_is_acl_selection();
    }
}

void EicielACLList::remove_selected_acl()
{
    Glib::RefPtr<Gtk::TreeSelection> selection_list_reference = _listview_acl.get_selection();
    Gtk::TreeModel::iterator iter = selection_list_reference->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row(*iter);
        if (row[_acl_list_model._removable]) {
            Gtk::TreeModel::Row row(*iter);
            _controller->remove_acl(Glib::ustring(row[_acl_list_model._entry_name]),
                ElementKind(row[_acl_list_model._entry_kind]));
        }
    }
}

void EicielACLList::there_is_no_acl_selection()
{
    _b_remove_acl.set_sensitive(false);
}

void EicielACLList::there_is_acl_selection()
{
    _b_remove_acl.set_sensitive(true);
}

void EicielACLList::add_non_selectable(
        Glib::RefPtr<Gtk::ListStore> ref_acl_list,
        Glib::ustring title,
    bool reading,
    bool writing,
    bool execution,
    ElementKind e)
{
    Gtk::TreeModel::iterator iter = ref_acl_list->append();
    Gtk::TreeModel::Row row(*iter);

    add_element(title, reading, writing, execution, e, row);
    row[_acl_list_model._removable] = false;
}

void EicielACLList::add_selectable(
        Glib::RefPtr<Gtk::ListStore> ref_acl_list,
        Glib::ustring title,
    bool reading,
    bool writing,
    bool execution,
    ElementKind e)
{
    Gtk::TreeModel::iterator iter = ref_acl_list->append();
    Gtk::TreeModel::Row row(*iter);

    add_element(title, reading, writing, execution, e, row);
    row[_acl_list_model._removable] = true;
}

void EicielACLList::add_element(Glib::ustring title,
    bool reading,
    bool writing,
    bool execution,
    ElementKind e,
    Gtk::TreeModel::Row& row)
{
    row[_acl_list_model._entry_kind] = e;
    row[_acl_list_model._icon] = get_proper_icon(e);
    row[_acl_list_model._entry_name] = title;
    row[_acl_list_model._reading_permission] = reading;
    row[_acl_list_model._writing_permission] = writing;
    row[_acl_list_model._execution_permission] = execution;
}

void EicielACLList::insert_before(const std::string& s, ElementKind new_element_kind, ElementKind before_element_kind, bool selectable)
{
    Gtk::TreeModel::Children children = _ref_acl_list->children();

    // Check first if was already inserted
    for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end(); iter++) {
        Gtk::TreeModel::Row row(*iter);
        if (row[_acl_list_model._entry_kind] == new_element_kind
                && row[_acl_list_model._entry_name] == s)
            return;
        // TODO: It should be possible to stop once before_element_kind has been seen
    }

    // If not found add it
    for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end(); iter++) {
        Gtk::TreeModel::Row row(*iter);
        if (row[_acl_list_model._entry_kind] == before_element_kind) {
            Gtk::TreeModel::Row new_row(*_ref_acl_list->insert(iter));
            add_element(s, /* reading */ true, /* writing */ true, /* execution */ true, new_element_kind, new_row);
            new_row[_acl_list_model._removable] = selectable;
            break;
        }
    }
}

void EicielACLList::populate_required_nondefault_entries()
{
    insert_before(_("Mask"), EK_MASK, EK_OTHERS, /* selectable */ false);
}

void EicielACLList::populate_required_default_entries()
{
    Gtk::TreeModel::Children children = _ref_acl_list->children();

    std::string user_owner, group_owner;
    for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end(); iter++) {
        Gtk::TreeModel::Row row(*iter);
        switch (row[_acl_list_model._entry_kind]) {
        default:
            break;
        case EK_USER:
            user_owner = Glib::ustring(row[_acl_list_model._entry_name]);
            break;
        case EK_GROUP:
            group_owner = Glib::ustring(row[_acl_list_model._entry_name]);
            break;
        case EK_DEFAULT_OTHERS:
            // We assume that an EK_DEFAULT_OTHERS means we are including default entries
            // so give up if already found.
            return;
        }
    }

    // EK_DEFAULT_OTHERS must be the last, so we handle it here manually
    add_non_selectable(_ref_acl_list, _("Default Other"),
        /* reading */ true,
        /* writing */ true,
        /* execution */ true,
        EK_DEFAULT_OTHERS);

    insert_before(_("Default Mask"), EK_DEFAULT_MASK, EK_DEFAULT_OTHERS, /* selectable */ false);

    insert_before(group_owner, EK_DEFAULT_GROUP, EK_DEFAULT_MASK, /* selectable */ false);
    insert_before(user_owner, EK_DEFAULT_USER, EK_DEFAULT_GROUP, /* selectable */ false);

    default_acl_are_being_edited(true);
}

void EicielACLList::remove_all_default_entries()
{
    Gtk::TreeModel::Children children = _ref_acl_list->children();
    // The ACL is empty, remove irrelevant entries now
    for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end();) {
        Gtk::TreeModel::Row row(*iter);
        switch (row[_acl_list_model._entry_kind]) {
        case EK_DEFAULT_ACL_USER:
        case EK_DEFAULT_ACL_GROUP:
        case EK_DEFAULT_USER:
        case EK_DEFAULT_GROUP:
        case EK_DEFAULT_MASK:
        case EK_DEFAULT_OTHERS:
            iter = _ref_acl_list->erase(iter);
            break;
        default:
            iter++;
        }
    }
}

void EicielACLList::insert_user(const std::string& s)
{
    populate_required_nondefault_entries();
    insert_before(s, EK_ACL_USER, EK_GROUP, /* selectable */ true);
}

void EicielACLList::insert_group(const std::string& s)
{
    populate_required_nondefault_entries();
    insert_before(s, EK_ACL_GROUP, EK_MASK, /* selectable */ true);
}

void EicielACLList::insert_default_user(const std::string& s)
{
    populate_required_default_entries();
    insert_before(s, EK_DEFAULT_ACL_USER, EK_DEFAULT_GROUP, /* selectable */ true);
}

void EicielACLList::insert_default_group(const std::string& s)
{
    populate_required_default_entries();
    insert_before(s, EK_DEFAULT_ACL_GROUP, EK_DEFAULT_MASK, /* selectable */ true);
}

bool EicielACLList::nondefault_acl_is_empty()
{
    Gtk::TreeModel::Children children = _ref_acl_list->children();

    for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end(); iter++) {
        Gtk::TreeModel::Row row(*iter);
        switch (row[_acl_list_model._entry_kind])
        {
            case EK_ACL_USER:
            case EK_ACL_GROUP:
                return false;
            default:
                break;
        }
    }

    return true;
}

void EicielACLList::remove_unneeded_entries()
{
    if (nondefault_acl_is_empty()) {
        Gtk::TreeModel::Children children = _ref_acl_list->children();
        // The ACL is empty, remove irrelevant entries now
        for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end();) {
            Gtk::TreeModel::Row row(*iter);
            switch (row[_acl_list_model._entry_kind]) {
            case EK_MASK:
                iter = _ref_acl_list->erase(iter);
                return;
            default:
                iter++;
            }
        }
    }
    // Default entries are handled in a manual fashion
}

void EicielACLList::remove_entry(const std::string &s, ElementKind e)
{
    Gtk::TreeModel::Children children = _ref_acl_list->children();

    for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end(); iter++) {
        Gtk::TreeModel::Row row(*iter);
        if (row[_acl_list_model._entry_kind] == e
                && row[_acl_list_model._entry_name] == s)
        {
            _ref_acl_list->erase(iter);
            break;
        }
    }

    remove_unneeded_entries();
}

Glib::RefPtr<Gdk::Pixbuf> EicielACLList::get_proper_icon(ElementKind e)
{
    switch (e) {
    case EK_USER:
        return _user_icon;
    case EK_GROUP:
        return _group_icon;
    case EK_OTHERS:
        return _others_icon;
    case EK_MASK:
        return _mask_icon;
    case EK_ACL_USER:
        return _user_icon_acl;
    case EK_ACL_GROUP:
        return _group_icon_acl;
    case EK_DEFAULT_USER:
        return _default_user_icon;
    case EK_DEFAULT_GROUP:
        return _default_group_icon;
    case EK_DEFAULT_OTHERS:
        return _default_others_icon;
    case EK_DEFAULT_ACL_USER:
        return _default_user_icon_acl;
    case EK_DEFAULT_ACL_GROUP:
        return _default_group_icon_acl;
    case EK_DEFAULT_MASK:
        return _default_mask_icon;
    default:
        return _others_icon;
    }
}

void EicielACLList::change_permissions(const Glib::ustring& str,
    PermissionKind p)
{
    if (_readonly_mode)
        return;

    Gtk::TreeModel::iterator i = _ref_acl_list->get_iter(str);
    Gtk::TreeModel::Row row(*i);

    switch (p) {
    case PK_READING:
        row[_acl_list_model._reading_permission] = !row[_acl_list_model._reading_permission];
        break;
    case PK_WRITING:
        row[_acl_list_model._writing_permission] = !row[_acl_list_model._writing_permission];
        break;
    case PK_EXECUTION:
        row[_acl_list_model._execution_permission] = !row[_acl_list_model._execution_permission];
        break;
    }

    _controller->update_acl_entry(row[_acl_list_model._entry_kind],
        Glib::ustring(row[_acl_list_model._entry_name]),
        row[_acl_list_model._reading_permission],
        row[_acl_list_model._writing_permission],
        row[_acl_list_model._execution_permission]);
}

void EicielACLList::can_edit_default_acl(bool b)
{
    // Show we hide it instead?
    _cb_modify_default_acl.set_sensitive(b);
}

void EicielACLList::default_acl_are_being_edited(bool b)
{
    _toggling_default_acl = true;
    _cb_modify_default_acl.set_active(b);
    _toggling_default_acl = false;
}

void EicielACLList::update_acl_ineffective(permissions_t effective_permissions, permissions_t effective_default_permissions)
{
    Gtk::TreeModel::Children children = _ref_acl_list->children();

    bool exist_ineffective_permissions = false;
    for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end(); iter++) {
        Gtk::TreeModel::Row row(*iter);
        switch (row[_acl_list_model._entry_kind]) {
        case EK_GROUP:
        case EK_ACL_USER:
        case EK_ACL_GROUP:
            row[_acl_list_model._reading_ineffective] = !effective_permissions.reading;
            row[_acl_list_model._writing_ineffective] = !effective_permissions.writing;
            row[_acl_list_model._execution_ineffective] = !effective_permissions.execution;
            exist_ineffective_permissions = exist_ineffective_permissions || ((!effective_permissions.reading && row[_acl_list_model._reading_permission]) || (!effective_permissions.writing && row[_acl_list_model._writing_permission]) || (!effective_permissions.execution && row[_acl_list_model._execution_permission]));
            break;
        case EK_DEFAULT_GROUP:
        case EK_DEFAULT_ACL_USER:
        case EK_DEFAULT_ACL_GROUP:
            row[_acl_list_model._reading_ineffective] = !effective_default_permissions.reading;
            row[_acl_list_model._writing_ineffective] = !effective_default_permissions.writing;
            row[_acl_list_model._execution_ineffective] = !effective_default_permissions.execution;
            exist_ineffective_permissions = exist_ineffective_permissions || ((!effective_default_permissions.reading && row[_acl_list_model._reading_permission]) || (!effective_default_permissions.writing && row[_acl_list_model._writing_permission]) || (!effective_default_permissions.execution && row[_acl_list_model._execution_permission]));
            break;
        // These don't change
        case EK_USER:
        case EK_OTHERS:
        case EK_MASK:
        case EK_DEFAULT_USER:
        case EK_DEFAULT_MASK:
        case EK_DEFAULT_OTHERS:
            break;
        }
    }
    set_exist_ineffective_permissions(exist_ineffective_permissions);
}

void EicielACLList::choose_acl(const std::string& s, ElementKind e)
{
    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_acl.get_model();
    Gtk::TreeModel::Children children = list_model->children();
    bool found = false;
    for (Gtk::TreeModel::Children::iterator iter = children.begin();
         (iter != children.end()) && !found; ++iter) {
        Gtk::TreeModel::Row row(*iter);
        if ((row[_acl_list_model._entry_kind] == e) && (row[_acl_list_model._entry_name] == s)) {
            found = true;
            Gtk::TreePath p = list_model->get_path(iter);
            _listview_acl.set_cursor(p);
            _listview_acl.scroll_to_row(p, 0.5);
            _listview_acl.grab_focus();
        }
    }
}

void EicielACLList::toggle_edit_default_acl()
{
    // This triggers some nasty reentrancy that we can stop here.
    if (_toggling_default_acl)
        return;
    _toggling_default_acl = true;

    // Because this is fired after the button has been pressed and release, the value
    // read is always the opposite. Perhaps there is a better way.
    bool changed = _controller->toggle_edit_default_acl(!_cb_modify_default_acl.get_active());
    if (!changed)
    {
        _cb_modify_default_acl.set_active(!_cb_modify_default_acl.get_active());
    }

    _toggling_default_acl = false;
}

void EicielACLList::acl_cell_data_func(Gtk::CellRenderer* rend,
    const Gtk::TreeModel::iterator& itr) {}

void EicielACLList::disable_default_acl_editing() 
{
    _middle_button_group.remove(_cb_modify_default_acl);
}

void EicielACLList::get_textual_representation(std::string& access_acl, std::string &default_acl)
{
    access_acl.clear();
    default_acl.clear();

    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_acl.get_model();
    Gtk::TreeModel::Children children = list_model->children();
    for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end(); iter++) {
        Gtk::TreeModel::Row row(*iter);
        permissions_t p(row[_acl_list_model._reading_permission],
            row[_acl_list_model._writing_permission],
            row[_acl_list_model._execution_permission]);

        // FIXME: the entry name may not exist and may have to be rendered using (id) syntax
        switch (row[_acl_list_model._entry_kind]) {
        case EK_USER:
            access_acl += "u::" + ACLManager::permission_to_str(p) + "\n";
            break;
        case EK_ACL_USER:
            access_acl += "u:" + row[_acl_list_model._entry_name] + ":" + ACLManager::permission_to_str(p) + "\n";
            break;
        case EK_GROUP:
            access_acl += "g::" + ACLManager::permission_to_str(p) + "\n";
            break;
        case EK_ACL_GROUP:
            access_acl += "g:" + row[_acl_list_model._entry_name] + ":" + ACLManager::permission_to_str(p) + "\n";
            break;
        case EK_MASK:
            access_acl += "m::" + ACLManager::permission_to_str(p) + "\n";
            break;
        case EK_OTHERS:
            access_acl += "o::" + ACLManager::permission_to_str(p) + "\n";
            break;

        case EK_DEFAULT_USER:
            default_acl += "u::" + ACLManager::permission_to_str(p) + "\n";
            break;
        case EK_DEFAULT_ACL_USER:
            default_acl += "u:" + row[_acl_list_model._entry_name] + ":" + ACLManager::permission_to_str(p) + "\n";
            break;
        case EK_DEFAULT_GROUP:
            default_acl += "g::" + ACLManager::permission_to_str(p) + "\n";
            break;
        case EK_DEFAULT_ACL_GROUP:
            default_acl += "g:" + row[_acl_list_model._entry_name] + ":" + ACLManager::permission_to_str(p) + "\n";
            break;
        case EK_DEFAULT_MASK:
            default_acl += "m::" + ACLManager::permission_to_str(p) + "\n";
            break;
        case EK_DEFAULT_OTHERS:
            default_acl += "o::" + ACLManager::permission_to_str(p) + "\n";
            break;
        }
    }
}

permissions_t EicielACLList::get_mask_permissions()
{
    permissions_t p(7);

    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_acl.get_model();
    Gtk::TreeModel::Children children = list_model->children();
    for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end(); iter++) {
        Gtk::TreeModel::Row row(*iter);
        if (row[_acl_list_model._entry_kind] == EK_MASK)
            return permissions_t(row[_acl_list_model._reading_permission],
                row[_acl_list_model._writing_permission],
                row[_acl_list_model._execution_permission]);
    }

    return p;
}

permissions_t EicielACLList::get_default_mask_permissions()
{
    permissions_t p(7);

    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_acl.get_model();
    Gtk::TreeModel::Children children = list_model->children();
    for (Gtk::TreeModel::iterator iter = children.begin(); iter != children.end(); iter++) {
        Gtk::TreeModel::Row row(*iter);
        if (row[_acl_list_model._entry_kind] == EK_DEFAULT_MASK)
            return permissions_t(row[_acl_list_model._reading_permission],
                row[_acl_list_model._writing_permission],
                row[_acl_list_model._execution_permission]);
    }

    return p;
}
