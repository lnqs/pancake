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

#include "pc_configparser.h"
#include <stdlib.h>
#include <pc_module.h>
#include "pc_modloader.h"

gboolean pc_configparser_parse(
		PcPanel* panel, const PcCommandlineOpts* cmdline_opts)
{
	/* This is only for testing -- real configfileparsing has to be added */
	GtkWidget* hbox = gtk_hbox_new(FALSE, 3);
	gtk_container_add(GTK_CONTAINER(panel), hbox);

	PancakePlugin* workspacenamemod = pc_modloader_load_plugin("workspacename");
	GtkWidget* workspacename = workspacenamemod->new_widget();
	gtk_box_pack_start(GTK_BOX(hbox), workspacename, FALSE, FALSE, 0);

	PancakePlugin* tasklistmod = pc_modloader_load_plugin("tasklist");
	GtkWidget* tasklist = tasklistmod->new_widget();
	gtk_box_pack_start(GTK_BOX(hbox), tasklist, TRUE, TRUE, 0);

	PancakePlugin* clockmod = pc_modloader_load_plugin("clock");
	GtkWidget* clock = clockmod->new_widget();
	gtk_box_pack_end(GTK_BOX(hbox), clock, FALSE, FALSE, 0);

	PancakeTheme* theme = pc_modloader_load_theme("defaulttheme");
	GtkStyle* style = GTK_STYLE(theme->new_style());
	pc_style_apply(style, GTK_WIDGET(panel));

	return TRUE;
}

