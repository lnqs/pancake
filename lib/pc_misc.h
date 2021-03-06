/**
 * Copyright (C) 2009 Simon Schönfeld <simon.schoenfeld@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef PC_MISC_H
#define PC_MISC_H

#include <gtk/gtk.h>

/* Holds the program's name as in argv[0].
 * It's only valid after pc_commandline_parse has been called */
extern const gchar* pc_program_invocation_name;

/* the currently active theme. This should be considered as read-only,
   unless you're about to change configfile-parsing */
extern GtkStyle* pc_theme;

/* The original gtk style, to be applied to dialogs */
extern GtkStyle* gtk_style;

/**
 * @brief Prints a dialog asking the user to fill a bugreport
 *
 * @param message_format the messageformat in printf-style
 * @param ... the data to print as in printf
 **/
void pc_bugdialog(const gchar* message_format, ...);

/**
 * @brief Resets a widgets style and it's children to GTKs default
 * 
 * @param widget the widget to reset
 **/
void pc_gtk_widget_reset_style_recursive(GtkWidget* widget);

#endif

