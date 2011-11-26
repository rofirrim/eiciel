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

#ifndef EICIEL_NAUTILUS_PAGE_HPP
#define EICIEL_NAUTILUS_PAGE_HPP

#define NAUTILUS_TYPE_EICIEL_PROPERTIES_PAGE	     (nautilus_eiciel_properties_page_get_type ())
#define NAUTILUS_EICIEL_PROPERTIES_PAGE(obj)	     (GTK_CHECK_CAST ((obj), NAUTILUS_TYPE_EICIEL_PROPERTIES_PAGE, NautilusEicielPropertiesPage))
#define NAUTILUS_EICIEL_PROPERTIES_PAGE_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), NAUTILUS_TYPE_EICIEL_PROPERTIES_PAGE, NautilusEicielPropertiesPageClass))
#define NAUTILUS_IS_EICIEL_PROPERTIES_PAGE(obj)	     (GTK_CHECK_TYPE ((obj), NAUTILUS_TYPE_EICIEL_PROPERTIES_PAGE))
#define NAUTILUS_IS_EICIEL_PROPERTIES_PAGE_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), NAUTILUS_TYPE_EICIEL_PROPERTIES_PAGE))

G_BEGIN_DECLS

typedef struct NautilusEicielPropertiesPageDetails NautilusEicielPropertiesPageDetails;

typedef struct {
	GtkVBox parent;
} NautilusEicielPropertiesPage;

typedef struct {
	GtkVBoxClass parent;
} NautilusEicielPropertiesPageClass;

G_END_DECLS

#endif // EICIEL_NAUTILUS_PAGE_HPP
