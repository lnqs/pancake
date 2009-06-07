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

#include "pc_misc.h"
#include <glib/gprintf.h>

void pc_bugdialog(const gchar* message_format, ...)
{
	va_list args;
	va_start(args, message_format);
	gchar* message;
	g_vasprintf(&message, message_format, args);
	va_end(args);
	
	GtkWidget* dialog = gtk_message_dialog_new_with_markup(
			NULL, 0, GTK_MESSAGE_WARNING, GTK_BUTTONS_CLOSE,
			"<big><b>An unexpected action occured</b></big>");
	gtk_message_dialog_format_secondary_markup(
			GTK_MESSAGE_DIALOG(dialog), "<b>%s</b>\n"
					"\n"
					"When this dialog is shown, pancake reached a state or "
					"received an event not expected when being programmed.\n"
					"This mustn't be as bad as it sounds -- if no strange "
					"behavour occures, it should be fine to just close this "
					"dialog, otherwise you should restart pancake now.\n"
					"In every case, we would ask to fill a bugreport to be "
					"able to fix this problem.", message);

	g_signal_connect_swapped(dialog, "response",
			G_CALLBACK(gtk_widget_destroy), dialog);

	gtk_widget_show_all(dialog);

	g_free(message);
}

