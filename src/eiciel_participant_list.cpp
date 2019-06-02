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
#include "eiciel_participant_list.hpp"
#include "eiciel_participant_list_controller.hpp"
#include "eiciel_participant_target.hpp"

// Constructor
EicielParticipantList::EicielParticipantList(EicielParticipantListController* cont)
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
    ,
    /* GUI */
    _main_box(Gtk::ORIENTATION_VERTICAL)
    , _participant_chooser()
    , _rb_acl_user(_("User"))
    , _rb_acl_group(_("Group"))
    , _cb_acl_default(_("Default participant"))
    , _filter_entry()
    , _listview_participants_container()
    , _listview_participants()
    , _below_participant_list()
    , _b_add_acl(_("Add participant to ACL"))
    , _b_add_acl_directory(_("Add participant to directory ACL"))
    , _b_add_acl_file(_("Add participant to file ACL"))
    , _advanced_features_expander(_("Advanced features"))
    , _advanced_features_box(Gtk::ORIENTATION_VERTICAL)
    , _participant_entry_label(_("Name of participant"))
    , _participant_entry_query_button(Gtk::Stock::FIND)
    , _cb_show_system_participants(_("Also show system participants"))

    /* Non GUI */
    , _pending_filter_updates(0)
    , _controller(cont) // Assign _controller to _window
{
    _controller->set_view(this);

    this->set_margin_top(12);
    this->set_margin_bottom(12);
    this->set_margin_start(12);
    this->set_margin_end(12);

    this->pack_start(_main_box);

    _main_box.set_spacing(4);

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

    // Group buttons of ACL kind
    Gtk::RadioButton::Group ACLKind = _rb_acl_user.get_group();
    _rb_acl_group.set_group(ACLKind);

    _filter_entry.set_placeholder_text(_("Filter participants"));
    _filter_entry.property_secondary_icon_name() = "gtk-clear";
    _filter_entry.signal_changed().connect(
        sigc::mem_fun(*this, &EicielParticipantList::filter_entry_text_changed));
    _filter_entry.signal_icon_press().connect(
        sigc::mem_fun(*this, &EicielParticipantList::on_clear_icon_pressed));
    _filter_entry.set_margin_left(12);

    // Row above the participants list
    _participant_chooser.pack_start(_rb_acl_user, Gtk::PACK_SHRINK);
    _participant_chooser.pack_start(_rb_acl_group, Gtk::PACK_SHRINK);
    _participant_chooser.pack_start(_cb_acl_default, Gtk::PACK_SHRINK);
    _participant_chooser.pack_end(_filter_entry, Gtk::PACK_SHRINK);

    // Participants list
    _ref_participants_list = Gtk::ListStore::create(_participant_list_model);
    _ref_participants_list_filter = Gtk::TreeModelFilter::create(_ref_participants_list);
    _ref_participants_list_filter->set_visible_func(
        sigc::mem_fun(*this, &EicielParticipantList::filter_participant_row));

    _listview_participants.set_headers_visible(false);
    _listview_participants.set_reallocate_redraws();
    _listview_participants.set_model(_ref_participants_list_filter);
    _listview_participants.append_column("", _participant_list_model._icon);
    _listview_participants.append_column(
        _("Participant"), _participant_list_model._participant_name);

    _listview_participants_container.add(_listview_participants);
    _listview_participants_container.set_policy(Gtk::POLICY_NEVER,
        Gtk::POLICY_AUTOMATIC);

    _participant_entry_box.set_spacing(4);
    _participant_entry_box.pack_start(_participant_entry_label, Gtk::PACK_SHRINK);
    _participant_entry_box.pack_start(_participant_entry);
    _participant_entry_box.pack_start(_participant_entry_query_button,
        Gtk::PACK_SHRINK);

    _participant_entry.signal_changed().connect(
        sigc::mem_fun(*this, &EicielParticipantList::participant_entry_box_changed));
    _participant_entry.signal_activate().connect(
        sigc::mem_fun(*this, &EicielParticipantList::participant_entry_box_activate));
    _participant_entry_query_button.set_sensitive(false);

    _participant_entry_query_button.signal_clicked().connect(
        sigc::mem_fun(*this, &EicielParticipantList::participant_entry_box_activate));

    // Row below the participant list
    _below_participant_list.set_spacing(4);
    _below_participant_list.pack_end(_b_add_acl, Gtk::PACK_SHRINK);

    _advanced_features_box.set_margin_left(12);
    _advanced_features_box.set_margin_right(12);
    _advanced_features_box.set_spacing(4);
    _advanced_features_box.pack_start(_participant_entry_box, Gtk::PACK_SHRINK);
    _advanced_features_box.pack_start(_cb_show_system_participants,
        Gtk::PACK_SHRINK);
    _advanced_features_expander.add(_advanced_features_box);

    _advanced_features_expander.property_expanded().signal_changed().connect(
        sigc::mem_fun(*this, &EicielParticipantList::advanced_features_box_expanded));

    _main_box.pack_start(_participant_chooser, Gtk::PACK_SHRINK);
    _main_box.pack_start(_listview_participants_container);
    _main_box.pack_start(_below_participant_list, Gtk::PACK_SHRINK);
    _main_box.pack_start(_advanced_features_expander, Gtk::PACK_SHRINK);

    _cb_show_system_participants.signal_toggled().connect(
        sigc::mem_fun(*this, &EicielParticipantList::toggle_system_show));

    _listview_participants_container.set_size_request(-1, 100);
    _listview_participants_container.set_shadow_type(Gtk::SHADOW_IN);

    // Drag & Drop stuff
#ifdef USING_GNOME2
    std::list<Gtk::TargetEntry> list_targets;
#else
    std::vector<Gtk::TargetEntry> list_targets;
#endif
    list_targets.push_back(Gtk::TargetEntry("participant_acl"));

    // Participants selection change event
    Glib::RefPtr<Gtk::TreeSelection> acl_list_participants_reference = _listview_participants.get_selection();
    acl_list_participants_reference->signal_changed().connect(
        sigc::mem_fun(*this, &EicielParticipantList::change_participant_selection));

    // Double click in participants list
    _listview_participants.signal_row_activated().connect(
        sigc::bind<0>(
            sigc::mem_fun(*this, &EicielParticipantList::participants_list_double_click),
            AddParticipantTarget::ADD_PARTICIPANT));

    // Add ACL button
    _b_add_acl.signal_clicked().connect(
        sigc::bind<0>(
            sigc::mem_fun(*this, &EicielParticipantList::add_selected_participant),
            AddParticipantTarget::ADD_PARTICIPANT));
    _b_add_acl_directory.signal_clicked().connect(
        sigc::bind<0>(
            sigc::mem_fun(*this, &EicielParticipantList::add_selected_participant),
            AddParticipantTarget::ADD_PARTICIPANT_TO_DIRECTORY));
    _b_add_acl_file.signal_clicked().connect(
        sigc::bind<0>(
            sigc::mem_fun(*this, &EicielParticipantList::add_selected_participant),
            AddParticipantTarget::ADD_PARTICIPANT_TO_FILE));

    there_is_no_participant_selection();

    _users_list = _controller->get_users_list();
    _groups_list = _controller->get_groups_list();

    _rb_acl_user.signal_clicked().connect(
        sigc::bind<std::set<std::string>*, ElementKind, Glib::RefPtr<Gdk::Pixbuf>,
            Glib::RefPtr<Gdk::Pixbuf>>(
            sigc::mem_fun(*this, &EicielParticipantList::fill_participants), &_users_list,
            EK_ACL_USER, _user_icon_acl, _default_user_icon_acl));

    _rb_acl_group.signal_clicked().connect(
        sigc::bind<std::set<std::string>*, ElementKind, Glib::RefPtr<Gdk::Pixbuf>,
            Glib::RefPtr<Gdk::Pixbuf>>(
            sigc::mem_fun(*this, &EicielParticipantList::fill_participants), &_groups_list,
            EK_ACL_GROUP, _group_icon_acl, _default_group_icon_acl));

    _cb_acl_default.signal_clicked().connect(
        sigc::mem_fun(*this, &EicielParticipantList::change_participant_kind));

    _rb_acl_user.set_active();
    _rb_acl_user.clicked();

    show_all();
}

EicielParticipantList::~EicielParticipantList() { }

void EicielParticipantList::change_participant_selection()
{
    Glib::RefPtr<Gtk::TreeSelection> selection_list_reference = _listview_participants.get_selection();
    Gtk::TreeModel::iterator iter = selection_list_reference->get_selected();
    if (!iter) {
        there_is_no_participant_selection();
    } else {
        if (_cb_acl_default.get_active()) {
            there_is_participant_selection_default();
        } else {
            there_is_participant_selection();
        }
    }
}

void EicielParticipantList::add_selected_participant(AddParticipantTarget target)
{
    Glib::RefPtr<Gtk::TreeSelection> selection_list_reference = _listview_participants.get_selection();
    Gtk::TreeModel::iterator iter = selection_list_reference->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row(*iter);
        _controller->add_acl_entry(
            target,
            Glib::ustring(row[_participant_list_model._participant_name]),
            ElementKind(row[_participant_list_model._entry_kind]),
            _cb_acl_default.get_active());
    }
}

void EicielParticipantList::there_is_no_participant_selection()
{
    _b_add_acl.set_sensitive(false);
    _b_add_acl_directory.set_sensitive(false);
    _b_add_acl_file.set_sensitive(false);
}

void EicielParticipantList::there_is_participant_selection()
{
    _b_add_acl.set_sensitive(true);
    _b_add_acl_directory.set_sensitive(true);
    _b_add_acl_file.set_sensitive(true);
}

void EicielParticipantList::there_is_participant_selection_default()
{
    _b_add_acl.set_sensitive(true);
    _b_add_acl_directory.set_sensitive(true);
    _b_add_acl_file.set_sensitive(false);
}

Glib::RefPtr<Gdk::Pixbuf> EicielParticipantList::get_proper_icon(ElementKind e)
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

void EicielParticipantList::fill_participants(std::set<std::string>* participants,
    ElementKind kind,
    Glib::RefPtr<Gdk::Pixbuf> normal_icon,
    Glib::RefPtr<Gdk::Pixbuf> default_icon)
{
    _ref_participants_list->clear();
    Gtk::TreeModel::iterator iter;

    bool enable_by_default = _cb_acl_default.get_active();

    for (std::set<std::string>::iterator i = participants->begin();
         i != participants->end(); i++) {
        iter = _ref_participants_list->append();
        Gtk::TreeModel::Row row(*iter);
        row[_participant_list_model._icon] = enable_by_default ? default_icon : normal_icon;
        row[_participant_list_model._participant_name] = *i;
        row[_participant_list_model._entry_kind] = kind;
    }
}

void EicielParticipantList::participants_list_double_click(AddParticipantTarget target,
    const Gtk::TreeModel::Path& p,
    Gtk::TreeViewColumn* c)
{
    if (_mode != MAIN_EDITOR)
        return;

    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_participants.get_model();
    Gtk::TreeModel::iterator iter = list_model->get_iter(p);

    if (iter) {
        Gtk::TreeModel::Row row(*iter);
        _controller->add_acl_entry(
            target,
            Glib::ustring(row[_participant_list_model._participant_name]),
            ElementKind(row[_participant_list_model._entry_kind]),
            _cb_acl_default.get_active());
    }
}

bool EicielParticipantList::enable_participant(const std::string& participant_name)
{
    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_participants.get_model();
    Gtk::TreeModel::Children children = list_model->children();
    bool found = false;

    for (Gtk::TreeModel::Children::iterator iter = children.begin();
         (iter != children.end()) && !found; ++iter) {
        Gtk::TreeModel::Row row(*iter);
        if (row[_participant_list_model._participant_name] == participant_name) {
            found = true;
            Gtk::TreePath p = list_model->get_path(iter);
            _listview_participants.set_cursor(p);
            _listview_participants.scroll_to_row(p, 0.5);
            _listview_participants.grab_focus();
        }
    }

    return found;
}

void EicielParticipantList::change_participant_kind()
{
    Glib::RefPtr<Gtk::TreeModel> list_model = _listview_participants.get_model();

    Gtk::TreeModel::Children children = list_model->children();

    bool enable_default = _cb_acl_default.get_active();

    for (Gtk::TreeModel::Children::iterator iter = children.begin();
         iter != children.end(); ++iter) {
        Gtk::TreeModel::Row row(*iter);
        if (_rb_acl_user.get_active()) {
            row[_participant_list_model._icon] = enable_default ? _default_user_icon_acl : _user_icon_acl;
        } else {
            row[_participant_list_model._icon] = enable_default ? _default_group_icon_acl : _group_icon_acl;
        }
    }

    change_participant_selection();
}

void EicielParticipantList::toggle_system_show()
{
    _controller->show_system_participants(
        _cb_show_system_participants.get_active());
    _users_list = _controller->get_users_list();
    _groups_list = _controller->get_groups_list();

    // Force the event without having to know the exact list kind
    if (_rb_acl_user.get_active())
        _rb_acl_user.clicked();

    if (_rb_acl_group.get_active())
        _rb_acl_group.clicked();
}

void EicielParticipantList::participant_entry_box_activate()
{
    bool participant_exists = false;

    participant_exists = enable_participant(_participant_entry.get_text());

    if (!participant_exists) {
        // Request for an explicit query in the system database
        if (_rb_acl_user.get_active()) {
            participant_exists = _controller->lookup_user(_participant_entry.get_text());
            if (participant_exists) {
                _users_list.insert(_participant_entry.get_text());
                _rb_acl_user.clicked();
                enable_participant(_participant_entry.get_text());
            }
        }

        if (_rb_acl_group.get_active()) {
            participant_exists = _controller->lookup_group(_participant_entry.get_text());
            if (participant_exists) {
                _groups_list.insert(_participant_entry.get_text());
                _rb_acl_group.clicked();
                enable_participant(_participant_entry.get_text());
            }
        }
    }

    if (!participant_exists) {
        _participant_entry.set_icon_from_stock(Gtk::Stock::DIALOG_ERROR);
        _participant_entry.set_icon_activatable(false);
        _participant_entry.set_icon_tooltip_text(_("Participant not found"));
    } else {
        _participant_entry.set_icon_from_pixbuf(Glib::RefPtr<Gdk::Pixbuf>(NULL));
    }
}

void EicielParticipantList::participant_entry_box_changed()
{
    _participant_entry_query_button.set_sensitive(
        _participant_entry.get_text_length() != 0);
}

bool EicielParticipantList::refilter()
{
    if (g_atomic_int_dec_and_test(&_pending_filter_updates)) {
        _ref_participants_list_filter->refilter();
    }
    return false;
}

void EicielParticipantList::on_clear_icon_pressed(Gtk::EntryIconPosition icon_position,
    const GdkEventButton* event)
{
    _filter_entry.set_text("");
}

void EicielParticipantList::filter_entry_text_changed()
{
    // wait filter_delay ms after each change to filter
    static const unsigned filter_delay = 500;

    g_atomic_int_inc(&_pending_filter_updates);
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &EicielParticipantList::refilter),
        filter_delay);
}

bool EicielParticipantList::filter_participant_row(
    const Gtk::TreeModel::const_iterator& iter)
{
    Glib::ustring filter_text = _filter_entry.get_text();
    if (!filter_text.empty()) {
        Glib::ustring current_text = (*iter)[_participant_list_model._participant_name];
        // check if current row text contains filter_text
        return current_text.find(filter_text) != Glib::ustring::npos;
    }
    return true;
}

void EicielParticipantList::set_readonly(bool b)
{
    this->set_sensitive(!b);
}

void EicielParticipantList::can_edit_default_acl(bool b)
{
    _cb_acl_default.set_sensitive(b);
}

void EicielParticipantList::set_mode(ParticipantListActionMode mode)
{
    if (mode == _mode)
        return;

    switch (mode) {
    default:
        // TODO: at least print a warning if crashing nautilus is not an option
        return;
    case ENCLOSED_FILES_EDITOR:
        _below_participant_list.remove(_b_add_acl);
        _below_participant_list.pack_end(_b_add_acl_file, Gtk::PACK_SHRINK);
        _below_participant_list.pack_end(_b_add_acl_directory, Gtk::PACK_SHRINK);
        break;
    case MAIN_EDITOR:
        _below_participant_list.remove(_b_add_acl_directory);
        _below_participant_list.remove(_b_add_acl_file);
        _below_participant_list.pack_end(_b_add_acl);
        break;
    }

    _mode = mode;
}

void EicielParticipantList::advanced_features_box_expanded()
{
    // This is a workaround for a weird behaviour observed with the expander
    // in which the hidden components are still sensitive once the expander
    // is folded. Hiding all the widgets manually seems to fix this.
    if (!_advanced_features_expander.get_expanded()) {
        _advanced_features_box.foreach([](Gtk::Widget& w) { w.hide(); });
    } else {
        _advanced_features_box.show_all();
    }
}
