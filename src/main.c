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

/* TODO: unify logging with the glib-logging-functions */
/* TODO: Test external building of modules */
/* TODO: static code analysis? */
/* TODO: valgrind? */
/* TODO: Add help for the modules */
/* TODO: Add trayarea-module */
/* TODO: Add example-config and install it in /usr/share/doc */

const gchar* pc_program_invocation_name;
GtkStyle* pc_theme = NULL;

static void pc_i18n_init()
{
	/* since partitially localisation sucks, this makes everything english,
	   until complete i18n-support is added */
	setlocale(LC_ALL, "C");
	gtk_disable_setlocale();
}

gboolean pc_set_style_hook(GSignalInvocationHint* ihint, guint n_param_values,
		const GValue* param_values, gpointer data)
{
	GtkWidget* widget = GTK_WIDGET(g_value_peek_pointer(param_values));
	GtkStyle* style = GTK_STYLE(data);
	gtk_widget_set_style(widget, style);
	return TRUE;
}

int main(int argc, char** argv)
{
	pc_program_invocation_name = argv[0];
	
	if(!pc_sighandler_init())
		return 2;
	
	pc_i18n_init();

	const PcCommandlineOpts* cmdline_opts = pc_commandline_parse(&argc, &argv);
	if(!cmdline_opts)
		return 1;
	if(!pc_modloader_init(cmdline_opts))
		return 2;
	
	GtkWidget* panel = pc_panel_new();

	if(!pc_configparser_parse((PcPanel*)panel, cmdline_opts))
	{
		pc_modloader_cleanup();
		return 2;
	}

	if(!pc_theme)
	{
		g_print("%s: No theme loaded\n", pc_program_invocation_name);
		pc_modloader_cleanup();
		return 2;
	}

	pc_style_apply(pc_theme, panel); /* sets style for everything created
									    already recursivly */
	g_signal_add_emission_hook(g_signal_lookup("realize", GTK_TYPE_WIDGET),
			0, &pc_set_style_hook, pc_theme, NULL); /* will set style for
													   everything that'll be 
													   created in future */
	pc_panel_set_border_padding(PC_PANEL(panel),
			pc_style_get_border_padding(pc_theme));

	gtk_widget_show_all(panel);
	gtk_main();
	gtk_widget_destroy(panel);
	
	pc_modloader_cleanup();

	return 0;
}

