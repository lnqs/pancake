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

#include <locale.h>
#include <gtk/gtk.h>
#include <pc_misc.h>
#include "pc_sighandler.h"
#include "pc_commandline.h"
#include "pc_modloader.h"
#include "pc_configparser.h"
#include "pc_panel.h"
#include "pc_style.h"

const gchar* pc_program_invocation_name;
GtkStyle* pc_theme = NULL;
GtkStyle* gtk_style = NULL;

static gboolean pc_log_message_to_ignore(
		const gchar* log_domain, const gchar* message)
{
	/* wnck likes to omit warnings about unknown atoms, but shouldn't unless
	   it's a _NET_*-atom, so we ignore the message */
	if(!g_strcmp0(log_domain, "Wnck"))
	{
		if(g_str_has_prefix(message, "Unhandled action type ") &&
				!g_str_has_prefix(message, "Unhandled action type _NET_"))
			return TRUE;
	}

	return FALSE;
}

static void pc_log_handler_terminal(const gchar* log_domain,
		GLogLevelFlags log_level, const gchar* message, gpointer data)
{
	if(pc_log_message_to_ignore(log_domain, message))
		return;

	g_print("%s: %s\n", pc_program_invocation_name, message);
}

static void pc_log_handler_dialog(const gchar* log_domain,
		GLogLevelFlags log_level, const gchar* message, gpointer data)
{
	static gboolean inside = FALSE;

	if(pc_log_message_to_ignore(log_domain, message))
		return;

	if(inside)
	{
		g_print("%s: %s\n", pc_program_invocation_name, message);
		return;
	}
	
	if(log_level & G_LOG_LEVEL_DEBUG || log_domain != NULL)
	{
		g_print("%s: %s\n", pc_program_invocation_name, message);
		return;
	}

	inside = TRUE;
	
	GtkMessageType type = G_LOG_LEVEL_INFO;
	const gchar* caption = "Information";

	if(log_level & G_LOG_LEVEL_WARNING)
	{
		type = GTK_MESSAGE_WARNING;
		caption = "Warning";
	}

	if((log_level & G_LOG_LEVEL_CRITICAL) || (log_level & G_LOG_LEVEL_ERROR))
	{
		caption = "Error";
		type = GTK_MESSAGE_ERROR;
	}
	
	GtkWidget* dialog = gtk_message_dialog_new_with_markup(
			NULL, 0, type, GTK_BUTTONS_CLOSE,
			"<big><b>%s</b></big>", caption);
	gtk_message_dialog_format_secondary_markup(
			GTK_MESSAGE_DIALOG(dialog), "%s", message);
	g_signal_connect_swapped(dialog, "response",
			G_CALLBACK(gtk_widget_destroy), dialog);
	pc_gtk_widget_reset_style_recursive(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

	inside = FALSE;
}

static void pc_i18n_init()
{
	/* since partitially localisation sucks, this makes everything english,
	   until complete i18n-support is added */
	setlocale(LC_ALL, "C");
	gtk_disable_setlocale();
}

static gboolean pc_set_style_hook(GSignalInvocationHint* ihint,
		guint n_param_values, const GValue* param_values, gpointer data)
{
	GtkWidget* widget = GTK_WIDGET(g_value_peek_pointer(param_values));
	GtkStyle* style = GTK_STYLE(data);
	gtk_widget_set_style(widget, style);
	return TRUE;
}

int main(int argc, char** argv)
{
	pc_program_invocation_name = argv[0];

	g_log_set_default_handler(&pc_log_handler_terminal, NULL);
	
	if(!pc_sighandler_init())
		return 2;
	
	pc_i18n_init();

	const PcCommandlineOpts* cmdline_opts = pc_commandline_parse(&argc, &argv);
	if(!cmdline_opts)
		return 1;
	if(!pc_modloader_init(cmdline_opts))
		return 2;
	
	if(cmdline_opts->module_help)
		pc_modloader_print_modulehelp();
	else
	{
		g_log_set_default_handler(&pc_log_handler_dialog, NULL);
		GtkWidget* panel = pc_panel_new();

		gtk_style = GTK_STYLE(g_object_ref(
				G_OBJECT(gtk_widget_get_style(panel))));

		if(!pc_configparser_parse((PcPanel*)panel, cmdline_opts))
		{
			pc_modloader_cleanup();
			g_object_unref(G_OBJECT(gtk_style));
			return 2;
		}

		pc_style_apply(pc_theme, panel); /* sets style for everything created
											already recursivly */
		g_signal_add_emission_hook(g_signal_lookup("realize", GTK_TYPE_WIDGET),
				0, &pc_set_style_hook, pc_theme, NULL); /*will set style for
														  everything that'll be 
														  created in future */
		pc_panel_set_border_padding(PC_PANEL(panel),
				pc_style_get_border_padding(pc_theme));

		gtk_widget_show_all(panel);
		gtk_main();
		gtk_widget_destroy(panel);
	}
	
	pc_modloader_cleanup();

	if(gtk_style)
		g_object_unref(G_OBJECT(gtk_style));

	return 0;
}

