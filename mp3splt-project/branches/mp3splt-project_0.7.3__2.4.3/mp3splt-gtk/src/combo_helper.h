/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2012 Alexandru Munteanu
 * Contact: io_fx@yahoo.fr
 *
 * http://mp3splt.sourceforge.net/
 *
 *********************************************************/

/**********************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 *********************************************************/

#ifndef COMBO_HELPER_H

#include <gtk/gtk.h>

GtkComboBox *ch_new_combo();
void ch_append_to_combo(GtkComboBox *combo, const gchar *text, gint value);
gint ch_get_active_value(GtkComboBox *combo);
gchar *ch_get_active_str_value(GtkComboBox *combo);
void ch_set_active_value(GtkComboBox *combo, gint value);
void ch_set_active_str_value(GtkComboBox *combo, gchar *new_value);

#define COMBO_HELPER_H
#endif

