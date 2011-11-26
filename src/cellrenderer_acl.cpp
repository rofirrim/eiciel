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
#include "cellrenderer_acl.hpp"
// #include <iostream>
#include <algorithm>

#define MARK_BACKGROUND_PROPERTY ("mark_background")

CellRendererACL::CellRendererACL()
    : Glib::ObjectBase (typeid(CellRendererACL)),
    Gtk::CellRendererToggle(),
    _mark_background(*this, MARK_BACKGROUND_PROPERTY, false)
{
}

// I don't want this in the header
static int default_indicator_size = 13;
static const int blank = 4;

void CellRendererACL::get_size_vfunc(Gtk::Widget& widget,
        const Gdk::Rectangle* cell_area,
        int * x_offset,
        int * y_offset,
        int * width,
        int * height) const 
{
#ifdef USING_GNOME2
    Glib::RefPtr<Gdk::Pixbuf> warning_icon = widget.render_icon(Gtk::Stock::DIALOG_WARNING, 
            Gtk::IconSize(Gtk::ICON_SIZE_SMALL_TOOLBAR), 
            "default");
#else
    Glib::RefPtr<Gdk::Pixbuf> warning_icon = widget.render_icon_pixbuf(Gtk::Stock::DIALOG_WARNING, 
            Gtk::IconSize(Gtk::ICON_SIZE_SMALL_TOOLBAR));
#endif

    const int checkbox_width = default_indicator_size;
    const int checkbox_height = default_indicator_size;
    const int inner_box_width = warning_icon->get_width() + blank + checkbox_width;
    const int inner_box_height = std::max(warning_icon->get_height(), checkbox_height);

    *width = inner_box_width;
    *height = inner_box_height;
}

void CellRendererACL::render_vfunc (
#ifdef USING_GNOME2
        const Glib::RefPtr<Gdk::Drawable>& drawable,
#else
        const Cairo::RefPtr<Cairo::Context>& cr,
#endif
        Gtk::Widget& widget,
        const Gdk::Rectangle& background_area,
        const Gdk::Rectangle& cell_area,
        const Gdk::Rectangle& expose_area,
        Gtk::CellRendererState flags)
{
#ifdef USING_GNOME2
    Glib::RefPtr<Gtk::Style> style = widget.get_style();
    Glib::RefPtr<Gdk::Window> window = Glib::RefPtr<Gdk::Window>::cast_dynamic(drawable);

    Gtk::StateType state = Gtk::STATE_NORMAL; 

    Glib::RefPtr<Gdk::Pixbuf> warning_icon = widget.render_icon(Gtk::Stock::DIALOG_WARNING, 
            Gtk::IconSize(Gtk::ICON_SIZE_SMALL_TOOLBAR), 
            "default");
#else
    Glib::RefPtr<Gtk::StyleContext> style_context = widget.get_style_context();

    style_context->set_state(Gtk::STATE_FLAG_NORMAL);

    Glib::RefPtr<Gdk::Pixbuf> warning_icon = widget.render_icon_pixbuf(Gtk::Stock::DIALOG_WARNING, 
            Gtk::IconSize(Gtk::ICON_SIZE_SMALL_TOOLBAR));
#endif

    /*
     * The size of the icon + 4 spacing pixels + checkbox of 13x13
     */

    const int checkbox_width = default_indicator_size;
    const int checkbox_height = default_indicator_size;
    const int inner_box_width = warning_icon->get_width() + blank + checkbox_width;
    const int inner_box_height = std::max(warning_icon->get_height(), checkbox_height);

    // std::cerr 
    //     << "--->" << std::endl
    //     << "inner_box_width= " << inner_box_width << std::endl
    //     << "inner_box_height= " << inner_box_height << std::endl
    //     << "cell_area_width= " << cell_area.get_width() << std::endl
    //     << "cell_area_height= " << cell_area.get_height() << std::endl
    //     << "<---" << std::endl;
    //     ;

    /*
     * Precondition: cell_area.get_width() >= inner_box_width
     *               cell_area.get_height() >= inner_box_height()
     *
     * Enforce this in the widget
     */

    int inner_x = (cell_area.get_width() - inner_box_width) / 2;
    int inner_y = (cell_area.get_height() - inner_box_height) / 2;

    inner_x = (inner_x < 0) ? 0 : inner_x;
    inner_y = (inner_y < 0) ? 0 : inner_y;

    inner_x += cell_area.get_x();
    inner_y += cell_area.get_y();

#ifdef USING_GNOME2
    Gtk::ShadowType shadow = Gtk::SHADOW_OUT;
    if (property_active())
    {
        shadow = Gtk::SHADOW_IN;

    }
#endif

    int checkbox_x = inner_x + warning_icon->get_width() + blank;
    int checkbox_y = inner_y + (warning_icon->get_height() - checkbox_height)/2; 

#ifdef USING_GNOME2
    style->paint_check(
            window,
            state,
            shadow,
            cell_area,
            widget,
            "checkbutton",
            checkbox_x, checkbox_y, checkbox_width, checkbox_height
            );
#else
    style_context->render_check(
            cr,
            checkbox_x, checkbox_y, checkbox_width, checkbox_height
            );
#endif

    if (property_active() && _mark_background.get_value())
    {
#ifdef USING_GNOME2
        int icon_x = inner_x;
        int icon_y = inner_y;

        Glib::RefPtr<Gdk::GC> graphic_context = Gdk::GC::create(drawable);

        drawable->draw_pixbuf(graphic_context,
                warning_icon,
                0, 0,
                icon_x, icon_y,
                -1, -1,
                Gdk::RGB_DITHER_NORMAL,
                0, 0);
#else
        Gdk::Cairo::set_source_pixbuf(cr, warning_icon, 0, 0);
        cr->paint();
#endif
    }
}


Glib::PropertyProxy<bool> CellRendererACL::mark_background()
{
    return Glib::PropertyProxy<bool>(this, MARK_BACKGROUND_PROPERTY);
}

