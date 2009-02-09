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
#include "sighandler.h"
#include "commandline.h"
#include "modloader.h"
#include "configparser.h"
#include "panel.h"

static void pc_i18n_init()
{
	/* since partitially localisation sucks, this makes everything english,
	   until complete i18n-support is added */
	setlocale(LC_ALL, "C");
	gtk_disable_setlocale();
}

int main(int argc, char** argv)
{
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
		return 2;

	gtk_widget_show_all(panel);
	gtk_main();
	gtk_widget_destroy(panel);
	
	pc_modloader_cleanup();

	return 0;
}

