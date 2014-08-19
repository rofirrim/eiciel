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
#include "eiciel_main_window.hpp"

// Constructor
EicielWindow::EicielWindow(EicielMainController* cont) 
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL),
    /* GUI */
    _main_box(Gtk::ORIENTATION_VERTICAL),
    _label_current_acl(_("<b>Current participants in ACL</b>")),
    _top_box(Gtk::ORIENTATION_VERTICAL),
    _listview_acl_container(),
    _listview_acl(),
    _middle_button_group(),
    _warning_icon(Gtk::Stock::DIALOG_WARNING, Gtk::IconSize(Gtk::ICON_SIZE_SMALL_TOOLBAR)),
    _bottom_label(_("There are ineffective permissions")),
    _b_remove_acl(_("Remove participant from ACL")),
    _tb_modify_default_acl(_("Edit default participants")),
    _label_participants(_("<b>Available participants</b>")),
    _bottom_box(Gtk::ORIENTATION_VERTICAL),
    _participant_chooser(),
    _rb_acl_user(_("User")),
    _rb_acl_group(_("Group")),
    _cb_acl_default(_("Default participant")),
    _filter_entry(),
    _listview_participants_container(),
    _listview_participants(),
    _below_participant_list(),
    _b_add_acl(_("Add participant to ACL")), 
    _advanced_features_expander(_("Advanced features")),
    _advanced_features_box(Gtk::ORIENTATION_VERTICAL),
    _participant_entry_label(_("Name of participant")),
    _participant_entry_query_button(Gtk::Stock::FIND),
    _cb_show_system_participants(_("Also show system participants")),

    /* Non GUI */
    _readonly_mode(false),
    _pending_filter_updates(0),
    _controller(cont) // Assign _controller to _window
{
    // Set the window to the controller
    _controller->_window = this;

    this->set_margin_top(12);
    this->set_margin_bottom(12);
    this->set_margin_start(12);
    this->set_margin_end(12);

    this->pack_start(_main_box);
    _main_box.set_spacing(4);

    _label_current_acl.set_use_markup(true);
    _label_current_acl.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_START);
    _main_box.pack_start(_label_current_acl, Gtk::PACK_SHRINK);

    _top_box.set_margin_start(12);
    _top_box.set_border_width(4);
    _top_box.set_spacing(4);
    _main_box.pack_start(_top_box);

    _label_participants.set_use_markup(true);
    _label_participants.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_START);
    _main_box.pack_start(_label_participants, Gtk::PACK_SHRINK);

    _bottom_box.set_margin_start(12);
    _bottom_box.set_border_width(4);
    _bottom_box.set_spacing(4);
    _main_box.pack_start(_bottom_box);

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
    _ref_acl_list = Gtk::ListStore::create(_acl_list_model);

    // Set columns in the ACL list
    _listview_acl.set_reallocate_redraws();
    _listview_acl.set_model(_ref_acl_list);
    _listview_acl.append_column("", _acl_list_model._icon);
    _listview_acl.append_column(_("Entry"), _acl_list_model._entry_name);

    CellRendererACL* renderRead = Gtk::manage(new CellRendererACL());
    int numColumns = _listview_acl.append_column(_("Read"), *renderRead);
    Gtk::TreeViewColumn* reading_column = _listview_acl.get_column(numColumns - 1);

    if (reading_column != NULL) 
    {
        reading_column->add_attribute(renderRead->property_active(), _acl_list_model._reading_permission);
        reading_column->add_attribute(renderRead->mark_background(), _acl_list_model._reading_ineffective);
        reading_column->set_cell_data_func(*renderRead, mem_fun(*this, &EicielWindow::acl_cell_data_func));
    }

    CellRendererACL* renderWrite = Gtk::manage(new CellRendererACL());
    numColumns = _listview_acl.append_column(_("Write"), *renderWrite);
    Gtk::TreeViewColumn* writing_column = _listview_acl.get_column(numColumns - 1);
    if (writing_column != NULL) 
    {
        writing_column->add_attribute(renderWrite->property_active(), _acl_list_model._writing_permission);
        writing_column->add_attribute(renderWrite->mark_background(), _acl_list_model._writing_ineffective);
        writing_column->set_cell_data_func(*renderWrite, mem_fun(*this, &EicielWindow::acl_cell_data_func));
    }

    CellRendererACL* renderExecute = Gtk::manage(new CellRendererACL());
    numColumns = _listview_acl.append_column(_("Execute"), *renderExecute);
    Gtk::TreeViewColumn* execution_column = _listview_acl.get_column(numColumns - 1);
    if (execution_column != NULL) 
    {
        execution_column->add_attribute(renderExecute->property_active(), _acl_list_model._execution_permission);
        execution_column->add_attribute(renderExecute->mark_background(), _acl_list_model._execution_ineffective);
        execution_column->set_cell_data_func(*renderExecute, mem_fun(*this, &EicielWindow::acl_cell_data_func));
    }

    // TODO - Implement recursion policies
#if 0
    // Recursion policy
    _recursion_policy_list = Gtk::ListStore::create(_recursion_policy_model);
    _dummy_recursion_policy_list = Gtk::ListStore::create(_recursion_policy_model);
    {
        Gtk::TreeModel::Row row = *(_recursion_policy_list->append());
        row[_recursion_policy_model._recursion_policy] = _("None");
        row = *(_recursion_policy_list->append());
        row[_recursion_policy_model._recursion_policy] = _("Only files");
        row = *(_recursion_policy_list->append());
        row[_recursion_policy_model._recursion_policy] = _("Only directories");
        row = *(_recursion_policy_list->append());
        row[_recursion_policy_model._recursion_policy] = _("Both files and directories");

        // Dummy
        row = *(_dummy_recursion_policy_list->append());
        row[_recursion_policy_model._recursion_policy] = _("None");
    }

    Gtk::CellRendererCombo* renderRecursion = Gtk::manage(new Gtk::CellRendererCombo());
    numColumns = _listview_acl.append_column(_("Recursion"), *renderRecursion);
    Gtk::TreeViewColumn* recursion_column = _listview_acl.get_column(numColumns - 1);

    recursion_column->add_attribute(renderRecursion->property_text(), _acl_list_model._current_recursion_policy);
    recursion_column->add_attribute(renderRecursion->property_model(), _acl_list_model._recursion_policies);
    renderRecursion->property_text_column() = 0;
    renderRecursion->property_editable() = true;
    renderRecursion->property_has_entry() = false;

    renderRecursion->signal_edited().connect( sigc::mem_fun(*this, &EicielWindow::recursion_policy_change) );
#endif
  
    // Aesthetic column completely useless
    _listview_acl.append_column("", _acl_list_model._empty);

    _listview_acl_container.add(_listview_acl);
    _listview_acl_container.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

    _listview_acl_container.set_size_request(-1, 150);
    _listview_acl_container.set_shadow_type(Gtk::SHADOW_IN);

    _top_box.pack_start(_listview_acl_container);

    // Add 'move up' 'move down' buttons
    _middle_button_group.pack_start(_warning_icon, Gtk::PACK_SHRINK);
    _middle_button_group.pack_start(_bottom_label, Gtk::PACK_SHRINK);
    _middle_button_group.pack_end(_b_remove_acl, Gtk::PACK_SHRINK);
    _middle_button_group.pack_end(_tb_modify_default_acl, Gtk::PACK_SHRINK);

    _top_box.pack_start(_middle_button_group, Gtk::PACK_SHRINK);

    // Group buttons of ACL kind
    Gtk::RadioButton::Group tipusACL = _rb_acl_user.get_group();
    _rb_acl_group.set_group(tipusACL);

    _filter_entry.set_placeholder_text(_("Filter participants"));
    _filter_entry.property_secondary_icon_name() = "gtk-clear";
    _filter_entry.signal_changed().connect(sigc::mem_fun(*this, &EicielWindow::filter_entry_text_changed));
    _filter_entry.signal_icon_press().connect(sigc::mem_fun(*this, &EicielWindow::on_clear_icon_pressed));
    _filter_entry.set_margin_left(12);

    // Row above the participants list
    _participant_chooser.pack_start(_rb_acl_user, Gtk::PACK_SHRINK);
    _participant_chooser.pack_start(_rb_acl_group, Gtk::PACK_SHRINK);
    _participant_chooser.pack_start(_cb_acl_default, Gtk::PACK_SHRINK);
    _participant_chooser.pack_end(_filter_entry, Gtk::PACK_SHRINK);

    // Participants list
    _ref_participants_list = Gtk::ListStore::create(_participant_list_model);
    _ref_participants_list_filter = Gtk::TreeModelFilter::create(_ref_participants_list);
    _ref_participants_list_filter->set_visible_func(sigc::mem_fun(*this, &EicielWindow::filter_participant_row));

    _listview_participants.set_headers_visible(false);
    _listview_participants.set_reallocate_redraws();
    _listview_participants.set_model(_ref_participants_list_filter);
    _listview_participants.append_column("", _participant_list_model._icon);
    _listview_participants.append_column(_("Participant"), _participant_list_model._participant_name);

    _listview_participants_container.add(_listview_participants);
    _listview_participants_container.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

    _participant_entry_box.set_spacing(4);
    _participant_entry_box.pack_start(_participant_entry_label, Gtk::PACK_SHRINK);
    _participant_entry_box.pack_start(_participant_entry);
    _participant_entry_box.pack_start(_participant_entry_query_button, Gtk::PACK_SHRINK);

    _participant_entry.signal_changed().connect(sigc::mem_fun(*this, &EicielWindow::participant_entry_box_changed));
    _participant_entry.signal_activate().connect(sigc::mem_fun(*this, &EicielWindow::participant_entry_box_activate));
    _participant_entry_query_button.set_sensitive(false);

    _participant_entry_query_button.signal_clicked().connect(sigc::mem_fun(*this, &EicielWindow::participant_entry_box_activate));

    // Row below the participant list
    _below_participant_list.pack_end(_b_add_acl, Gtk::PACK_SHRINK);

    _advanced_features_box.set_margin_left(12);
    _advanced_features_box.set_margin_right(12);
    _advanced_features_box.set_spacing(4);
    _advanced_features_box.pack_start(_participant_entry_box, Gtk::PACK_SHRINK);
    _advanced_features_box.pack_start(_cb_show_system_participants, Gtk::PACK_SHRINK);
    _advanced_features_expander.add(_advanced_features_box);
    
    _bottom_box.pack_start(_participant_chooser, Gtk::PACK_SHRINK);
    _bottom_box.pack_start(_listview_participants_container);
    _bottom_box.pack_start(_below_participant_list, Gtk::PACK_SHRINK);
    _bottom_box.pack_start(_advanced_features_expander, Gtk::PACK_SHRINK);

    _cb_show_system_participants.signal_toggled().connect(sigc::mem_fun(*this, &EicielWindow::toggle_system_show));

    _listview_participants_container.set_size_request(-1, 100);
    _listview_participants_container.set_shadow_type(Gtk::SHADOW_IN);

    // Drag & Drop stuff
#ifdef USING_GNOME2
    std::list<Gtk::TargetEntry> list_targets;
#else
    std::vector<Gtk::TargetEntry> list_targets;
#endif
    list_targets.push_back( Gtk::TargetEntry("participant_acl") );

    // _listview_participants es the source of the drag
    _listview_participants.drag_source_set(list_targets);

    _listview_participants.signal_drag_begin().connect( 
            sigc::mem_fun(*this, &EicielWindow::start_drag_and_drop)
            );

    _listview_participants.signal_drag_data_get().connect( 
            sigc::mem_fun(*this, &EicielWindow::set_value_drag_and_drop)
            );

    // _listview_acl is the target of the drag
    _listview_acl.drag_dest_set(list_targets);

    _listview_acl.signal_drag_data_received().connect( 
            sigc::mem_fun(*this, &EicielWindow::get_value_drag_and_drop)
            );

    // Signal binding
    Glib::RefPtr<Gtk::TreeSelection> acl_list_selection_reference = _listview_acl.get_selection();

    // Filter function this time only for _readonly_mode
    acl_list_selection_reference->set_select_function( 
            sigc::mem_fun(*this, &EicielWindow::acl_selection_function) 
            );
    
    // ACL selection change event
    acl_list_selection_reference->signal_changed().connect(
            sigc::mem_fun(*this, &EicielWindow::acl_selection_change)
            );

    // Participants selection change event
    Glib::RefPtr<Gtk::TreeSelection> acl_list_participants_reference = _listview_participants.get_selection();
    acl_list_participants_reference->signal_changed().connect(
            sigc::mem_fun(*this, &EicielWindow::change_participant_selection)
            );
    
    // Double click in ACL list
    _listview_acl.signal_row_activated().connect(
            sigc::mem_fun(*this, &EicielWindow::acl_list_double_click)
            );

    // Double click in participants list
    _listview_participants.signal_row_activated().connect(
            sigc::mem_fun(*this, &EicielWindow::participants_list_double_click)
            );

    // Remove ACL button
    _b_remove_acl.signal_clicked().connect(
            sigc::mem_fun(*this, &EicielWindow::remove_selected_acl)
            );

    // Add ACL button
    _b_add_acl.signal_clicked().connect(
            sigc::mem_fun(*this, &EicielWindow::add_selected_participant)
            );

    // Change of default acl toggle button
    _tb_modify_default_acl.signal_clicked().connect (
            sigc::mem_fun(*_controller, &EicielMainController::change_default_acl)
            );

    // Column edition event
    Gtk::TreeViewColumn* col = _listview_acl.get_column(2);
#ifdef USING_GNOME2
    Gtk::CellRenderer* render = col->get_first_cell_renderer(); // Only one
#else
    Gtk::CellRenderer* render = col->get_first_cell(); // Only one
#endif
    CellRendererACL* toggle_render = dynamic_cast<CellRendererACL*>(render);

    toggle_render->signal_toggled().connect(
            sigc::bind<PermissionKind>( sigc::mem_fun(*this, &EicielWindow::change_permissions), PK_READING)
            );

    col = _listview_acl.get_column(3);
#ifdef USING_GNOME2
    render = col->get_first_cell_renderer();
#else
    render = col->get_first_cell();
#endif
    toggle_render = dynamic_cast<CellRendererACL*>(render);

    toggle_render->signal_toggled().connect(
            sigc::bind<PermissionKind>( sigc::mem_fun(*this, &EicielWindow::change_permissions), PK_WRITING)
            );

    col = _listview_acl.get_column(4);
#ifdef USING_GNOME2
    render = col->get_first_cell_renderer();
#else
    render = col->get_first_cell();
#endif
    toggle_render = dynamic_cast<CellRendererACL*>(render);

    toggle_render->signal_toggled().connect(
            sigc::bind<PermissionKind>( sigc::mem_fun(*this, &EicielWindow::change_permissions), PK_EXECUTION)
            );

    there_is_no_file();
    there_is_no_acl_selection();
    there_is_no_participant_selection();

    _users_list = _controller->get_users_list();
    _groups_list = _controller->get_groups_list();

    _rb_acl_user.signal_clicked().connect(
            sigc::bind< set<string>*,
            ElementKind, 
            Glib::RefPtr<Gdk::Pixbuf>,
            Glib::RefPtr<Gdk::Pixbuf> > (
                sigc::mem_fun(*this, &EicielWindow::fill_participants),
                &_users_list,
                EK_ACL_USER, 
                _user_icon_acl, 
                _default_user_icon_acl)
            );

    _rb_acl_group.signal_clicked().connect(
            sigc::bind< set<string>*,
            ElementKind, 
            Glib::RefPtr<Gdk::Pixbuf>,
            Glib::RefPtr<Gdk::Pixbuf> > (
                sigc::mem_fun(*this, &EicielWindow::fill_participants),
                &_groups_list,
                EK_ACL_GROUP, 
                _group_icon_acl, 
                _default_group_icon_acl)
            );

    _cb_acl_default.signal_clicked().connect(
            sigc::mem_fun(*this, &EicielWindow::change_participant_kind)
            );

    _rb_acl_user.set_active();
    _rb_acl_user.clicked();

    show_all();

    show_exclamation_mark(false);
}


void EicielWindow::show_exclamation_mark(bool b)
{
    if (b)
    {
        _bottom_label.show();
        _warning_icon.show();
    }
    else
    {
        _warning_icon.hide();
        _bottom_label.hide();
    }
}

EicielWindow::~EicielWindow()
{
    delete _controller;
}

void EicielWindow::initialize(string s)
{
    _controller->open_file(s);
}

void EicielWindow::set_active(bool b)
{
    _main_box.set_sensitive(b);
}

void EicielWindow::set_readonly(bool b)
{
    _readonly_mode = b;
    if (b)
    {
        _tb_modify_default_acl.set_sensitive(false);
    }
    _bottom_box.set_sensitive(!b);
}

bool EicielWindow::acl_selection_function(const Glib::RefPtr<Gtk::TreeModel>& model,
        const Gtk::TreeModel::Path& path, bool)
{
    return !_readonly_mode;
}

void EicielWindow::there_is_no_file()
{
    _main_box.set_sensitive(false);
}

void EicielWindow::empty_acl_list()
{
    _ref_acl_list->clear();
}

void EicielWindow::acl_selection_change()
{
    Glib::RefPtr<Gtk::TreeSelection> selection_list_reference = _listview_acl.get_selection();
    Gtk::TreeModel::iterator iter = selection_list_reference->get_selected();
    if (!iter || _readonly_mode)
    {
        there_is_no_acl_selection();
    }
    else
    {
        if ( !(*iter)[_acl_list_model._removable] )
            there_is_no_acl_selection();
        else
            there_is_acl_selection();
    }
}

void EicielWindow::change_participant_selection()
{
    Glib::RefPtr<Gtk::TreeSelection> selection_list_reference = _listview_participants.get_selection();
    Gtk::TreeModel::iterator iter = selection_list_reference->get_selected();
    if (!iter)
    {
        there_is_no_participant_selection();
    }
    else
    {
        there_is_participant_selection();
    }
}

void EicielWindow::remove_selected_acl()
{
    Glib::RefPtr<Gtk::TreeSelection> selection_list_reference = _listview_acl.get_selection();
    Gtk::TreeModel::iterator iter = selection_list_reference->get_selected();
    if (iter)
    {
        Gtk::TreeModel::Row row(*iter);
        if (row[_acl_list_model._removable])
        {
            Gtk::TreeModel::Row row(*iter);
            _controller->remove_acl(Glib::ustring(row[_acl_list_model._entry_name]), 
                    ElementKind(row[_acl_list_model._entry_kind]));
        }
    }
}

void EicielWindow::add_selected_participant()
{
    Glib::RefPtr<Gtk::TreeSelection> selection_list_reference = _listview_participants.get_selection();
    Gtk::TreeModel::iterator iter = selection_list_reference->get_selected();
    if (iter)
    {
        Gtk::TreeModel::Row row(*iter);
        _controller->add_acl_entry(Glib::ustring(row[_participant_list_model._participant_name]), 
                ElementKind(row[_participant_list_model._entry_kind]), _cb_acl_default.get_active());
    }
}

void EicielWindow::there_is_no_acl_selection()
{
    _b_remove_acl.set_sensitive(false);
}

void EicielWindow::there_is_no_participant_selection()
{
    _b_add_acl.set_sensitive(false);
}

void EicielWindow::there_is_acl_selection()
{
    _b_remove_acl.set_sensitive(true);
}

void EicielWindow::there_is_participant_selection()
{
    _b_add_acl.set_sensitive(true);
}

void EicielWindow::add_non_selectable(Glib::ustring title, 
        bool reading, bool writing, bool execution, ElementKind e,
        bool effective_reading, bool effective_writing, bool effective_execution)
{
    Gtk::TreeModel::iterator iter = _ref_acl_list->append();
    Gtk::TreeModel::Row row(*iter);

    add_element(title, reading, writing, execution, e, row, 
            effective_reading, effective_writing, effective_execution,
            _controller->is_directory());
    row[_acl_list_model._removable] = false;
}

void EicielWindow::add_selectable(Glib::ustring title, 
        bool reading, bool writing, bool execution, ElementKind e,
        bool effective_reading, bool effective_writing, bool effective_execution)
{
    Gtk::TreeModel::iterator iter = _ref_acl_list->append();
    Gtk::TreeModel::Row row(*iter);

    add_element(title, reading, writing, execution, e, row,
            effective_reading, effective_writing, effective_execution,
            _controller->is_directory());
    row[_acl_list_model._removable] = true;
}

void EicielWindow::add_element(Glib::ustring title, 
        bool reading, bool writing, bool execution,
        ElementKind e, Gtk::TreeModel::Row& row,
        bool effective_reading, 
        bool effective_writing, 
        bool effective_execution,
        bool can_be_recursed)
{
    row[_acl_list_model._entry_kind] = e;
    row[_acl_list_model._icon] = get_proper_icon(e);
    row[_acl_list_model._entry_name] = title;
    row[_acl_list_model._reading_permission] = reading;
    row[_acl_list_model._writing_permission] = writing;
    row[_acl_list_model._execution_permission] = execution;

    row[_acl_list_model._reading_ineffective] = !effective_reading;
    row[_acl_list_model._writing_ineffective] = !effective_writing;
    row[_acl_list_model._execution_ineffective] = !effective_execution;

    // TODO - Implement recursion policies
#if 0
    row[_acl_list_model._current_recursion_policy] = _("None");
    if (can_be_recursed)
    {
        row[_acl_list_model._recursion_policies] = _recursion_policy_list;
    }
    else
    {
        row[_acl_list_model._recursion_policies] = _dummy_recursion_policy_list;
    }
#endif
}

Glib::RefPtr<Gdk::Pixbuf> EicielWindow::get_proper_icon(ElementKind e)
{
    switch(e)
    {
        case EK_USER : 
            return _user_icon;
        case EK_GROUP :
            return _group_icon;
        case EK_OTHERS :
            return _others_icon;
        case EK_MASK :
            return _mask_icon;
        case EK_ACL_USER : 
            return _user_icon_acl;
        case EK_ACL_GROUP :
            return _group_icon_acl;
        case EK_DEFAULT_USER :
            return _default_user_icon;
        case EK_DEFAULT_GROUP :
            return _default_group_icon;
        case EK_DEFAULT_OTHERS :
            return _default_others_icon;
        case EK_DEFAULT_ACL_USER :
            return _default_user_icon_acl;
        case EK_DEFAULT_ACL_GROUP :
            return _default_group_icon_acl;
        case EK_DEFAULT_MASK :
            return _default_mask_icon;
        default :
            return _others_icon;
    }
}

void EicielWindow::set_filename(string filename)
{
    _main_box.set_sensitive(true);
}

void EicielWindow::change_permissions(const Glib::ustring& str, PermissionKind p)
{
    Gtk::TreeModel::iterator i = _ref_acl_list->get_iter(str);
    Gtk::TreeModel::Row row(*i);

    if (_readonly_mode) return;

    switch (p)
    {
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
            Glib::ustring(row[_acl_list_model._entry_name]), row[_acl_list_model._reading_permission],
            row[_acl_list_model._writing_permission], row[_acl_list_model._execution_permission]);
}

void EicielWindow::enable_default_acl_button(bool b)
{
    _tb_modify_default_acl.set_sensitive(b);
    _cb_acl_default.set_sensitive(b);
    if (!b)
    {
        _cb_acl_default.set_active(false);
    }
}

void EicielWindow::there_is_default_acl(bool b)
{
    _tb_modify_default_acl.set_active(b);
}

bool EicielWindow::give_default_acl()
{
    return _tb_modify_default_acl.get_active();
}

void EicielWindow::fill_participants(set<string>* participants,
        ElementKind kind, 
        Glib::RefPtr<Gdk::Pixbuf> normal_icon,
        Glib::RefPtr<Gdk::Pixbuf> default_icon)
{
    _ref_participants_list->clear();
    Gtk::TreeModel::iterator iter;

    bool enable_by_default = _cb_acl_default.get_active();

    for (set<string>::iterator i = participants->begin(); i != participants->end(); i++)
    {
        iter = _ref_participants_list->append();
        Gtk::TreeModel::Row row(*iter);
        row[_participant_list_model._icon] = 
            enable_by_default ? default_icon : normal_icon;
        row[_participant_list_model._participant_name] = *i;
        row[_participant_list_model._entry_kind] = kind;
    }
}

bool EicielWindow::opened_file()
{
    return _controller->opened_file();
}

Glib::ustring EicielWindow::last_error()
{
    return _controller->last_error();
}

void EicielWindow::participants_list_double_click(const Gtk::TreeModel::Path& p,
        Gtk::TreeViewColumn* c)
{
    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_participants.get_model();
    Gtk::TreeModel::iterator iter = list_model->get_iter(p);

    if (iter)
    {
        Gtk::TreeModel::Row row(*iter);
        _controller->add_acl_entry(Glib::ustring(row[_participant_list_model._participant_name]), 
                ElementKind(row[_participant_list_model._entry_kind]), _cb_acl_default.get_active());
    }
}

void EicielWindow::acl_list_double_click(const Gtk::TreeModel::Path& p,
        Gtk::TreeViewColumn* c)
{
    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_acl.get_model();
    Gtk::TreeModel::iterator iter = list_model->get_iter(p);

    if (_readonly_mode) return;

    if (iter)
    {
        Gtk::TreeModel::Row row(*iter);
        if (row[_acl_list_model._removable])
        {
            _controller->remove_acl(Glib::ustring(row[_acl_list_model._entry_name]), 
                    ElementKind(row[_acl_list_model._entry_kind]));
        }
    }
}

void EicielWindow::choose_acl(string s, ElementKind e)
{
    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_acl.get_model();
    Gtk::TreeModel::Children children = list_model->children();
    bool found = false;
    for(Gtk::TreeModel::Children::iterator iter = children.begin(); 
            (iter != children.end()) && !found; ++iter)
    {
        Gtk::TreeModel::Row row(*iter);
        if ((row[_acl_list_model._entry_kind] == e) &&
                (row[_acl_list_model._entry_name] == s))
        {
            found = true;
            Gtk::TreePath p = list_model->get_path(iter);
            _listview_acl.set_cursor(p);
            _listview_acl.scroll_to_row(p, 0.5);
            _listview_acl.grab_focus();
        }
    }
}

bool EicielWindow::enable_participant(string participant_name)
{
    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_participants.get_model();
    Gtk::TreeModel::Children children = list_model->children();
    bool found = false;

    for(Gtk::TreeModel::Children::iterator iter = children.begin(); 
            (iter != children.end()) && !found; ++iter)
    {
        Gtk::TreeModel::Row row(*iter);
        if (row[_participant_list_model._participant_name] == participant_name)
        {
            found = true;
            Gtk::TreePath p = list_model->get_path(iter);
            _listview_participants.set_cursor(p);
            _listview_participants.scroll_to_row(p, 0.5);
            _listview_participants.grab_focus();
        }
    }

    return found;
}


void EicielWindow::set_value_drag_and_drop(const Glib::RefPtr<Gdk::DragContext>&, 
        Gtk::SelectionData& selection_data, guint, guint)
{
    selection_data.set("participant_acl", "");
}

void EicielWindow::get_value_drag_and_drop(const Glib::RefPtr<Gdk::DragContext>& context, 
        int, int, const Gtk::SelectionData& selection_data, guint, guint time)
{
    add_selected_participant();
    context->drag_finish(true, false, time);
}

void EicielWindow::start_drag_and_drop(const Glib::RefPtr<Gdk::DragContext>& context)
{
    Glib::RefPtr<Gtk::TreeSelection> selection_list_reference = _listview_participants.get_selection();
    Gtk::TreeModel::iterator iter = selection_list_reference->get_selected();
    if (iter)
    {
        Gtk::TreeModel::Row row(*iter);
        Glib::RefPtr<Gdk::Pixbuf> imatge = row[_participant_list_model._icon];
        context->set_icon(imatge, -4, -4);
    }
}

void EicielWindow::change_participant_kind()
{
    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_participants.get_model();

    Gtk::TreeModel::Children children = list_model->children();

    bool enable_default = _cb_acl_default.get_active();

    for(Gtk::TreeModel::Children::iterator iter = children.begin(); 
            iter != children.end(); ++iter)
    {
        Gtk::TreeModel::Row row(*iter);
        if (_rb_acl_user.get_active())
        {
            row[_participant_list_model._icon] = 
                enable_default ? _default_user_icon_acl : _user_icon_acl;
        }
        else
        {
            row[_participant_list_model._icon] = 
                enable_default ? _default_group_icon_acl : _group_icon_acl;
        }
    }
}

void EicielWindow::toggle_system_show()
{
    _controller->show_system_participants(_cb_show_system_participants.get_active());
    _users_list = _controller->get_users_list();
    _groups_list = _controller->get_groups_list();

    // Force the event without having to know the exact list kind
    if (_rb_acl_user.get_active()) 
        _rb_acl_user.clicked();

    if (_rb_acl_group.get_active()) 
        _rb_acl_group.clicked();
}


void EicielWindow::acl_cell_data_func(Gtk::CellRenderer* rend, 
        const Gtk::TreeModel::iterator& itr)
{
}

void EicielWindow::recursion_policy_change(const Glib::ustring& path_string, const Glib::ustring& new_text)
{
    Gtk::TreePath path(path_string);

    Gtk::TreeModel::iterator iter = _ref_acl_list->get_iter(path);
    if (iter)
    {
        Gtk::TreeRow row = *iter;
        row[_acl_list_model._current_recursion_policy] = new_text;
    }
}

void EicielWindow::participant_entry_box_activate()
{
    bool participant_exists = false;

    participant_exists = enable_participant(_participant_entry.get_text());

    if (!participant_exists)
    {
        // Request for an explicit query in the system database
        if (_rb_acl_user.get_active()) 
        {
            participant_exists = _controller->lookup_user(_participant_entry.get_text());
            if (participant_exists)
            {
                _users_list.insert(_participant_entry.get_text());
                _rb_acl_user.clicked();
                enable_participant(_participant_entry.get_text());
            }
        }

        if (_rb_acl_group.get_active()) 
        {
            participant_exists = _controller->lookup_group(_participant_entry.get_text());
            if (participant_exists)
            {
                _groups_list.insert(_participant_entry.get_text());
                _rb_acl_group.clicked();
                enable_participant(_participant_entry.get_text());
            }
        }
    }

    if (!participant_exists)
    {
        _participant_entry.set_icon_from_stock(Gtk::Stock::DIALOG_ERROR);
        _participant_entry.set_icon_activatable(false);
        _participant_entry.set_icon_tooltip_text(_("Participant not found"));
    }
    else
    {
        _participant_entry.set_icon_from_pixbuf(Glib::RefPtr<Gdk::Pixbuf>(NULL));
    }
}

void EicielWindow::participant_entry_box_changed()
{
    _participant_entry_query_button.set_sensitive( _participant_entry.get_text_length() != 0 );
}

bool EicielWindow::refilter()
{
    if (g_atomic_int_dec_and_test(&_pending_filter_updates))
    {
        _ref_participants_list_filter->refilter();
    }
    return false;
}

void EicielWindow::on_clear_icon_pressed(Gtk::EntryIconPosition icon_position, const GdkEventButton* event)
{
    _filter_entry.set_text("");
}

void EicielWindow::filter_entry_text_changed()
{
    // wait filter_delay ms after each change to filter
    static const unsigned filter_delay = 500;

    g_atomic_int_inc(&_pending_filter_updates);
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &EicielWindow::refilter), filter_delay);
}

bool EicielWindow::filter_participant_row(const Gtk::TreeModel::const_iterator& iter)
{
    Glib::ustring filter_text = _filter_entry.get_text();
    if (!filter_text.empty())
    {
        Glib::ustring current_text = (*iter)[_participant_list_model._participant_name];
        // check if current row text contains filter_text
        return current_text.find(filter_text) != Glib::ustring::npos;
    }
    return true;
}
