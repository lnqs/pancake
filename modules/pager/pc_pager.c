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

#include <gtk/gtk.h>
#include <pc_module.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

gboolean pc_pager_scroll_event(
		GtkWidget* widget, GdkEventScroll* event, gpointer data)
{
	if(event->direction == GDK_SCROLL_UP)
	{
		wnck_workspace_activate(wnck_screen_get_workspace_neighbor(
				wnck_screen_get_default(), wnck_screen_get_active_workspace(
				wnck_screen_get_default()), WNCK_MOTION_LEFT), event->time);
	}
	else if(event->direction == GDK_SCROLL_DOWN)
	{
		wnck_workspace_activate(wnck_screen_get_workspace_neighbor(
				wnck_screen_get_default(), wnck_screen_get_active_workspace(
				wnck_screen_get_default()), WNCK_MOTION_RIGHT), event->time);
	}
	
	return FALSE;
}

static int pc_pager_parse_mode(
		Config* cfg, ConfigOption* opt, const char* value, void* result)
{
	if(!g_strcmp0(value, "name"))
		*(glong*)result = WNCK_PAGER_DISPLAY_NAME;
	else if(!g_strcmp0(value, "content"))
		*(glong*)result = WNCK_PAGER_DISPLAY_CONTENT;
	else
	{
		cfg_error(cfg, "invalid value for option %s: %s", opt->name, value);
		return -1;
	}
	
	return 0;
}

static GtkWidget* pc_pager_instantiate(Config* config)
{
	GtkWidget* pager = wnck_pager_new(wnck_screen_get_default());
	wnck_pager_set_shadow_type(WNCK_PAGER(pager), GTK_SHADOW_NONE);
	wnck_pager_set_display_mode(WNCK_PAGER(pager), cfg_getint(config, "mode"));
	wnck_pager_set_show_all(
			WNCK_PAGER(pager), cfg_getbool(config, "all_workspaces"));
	wnck_pager_set_n_rows(WNCK_PAGER(pager), cfg_getint(config, "rows"));

	g_signal_connect(G_OBJECT(pager), "scroll-event",
			G_CALLBACK(&pc_pager_scroll_event), NULL);

	return pager;
}

static ConfigOption pc_pager_options[] = {
	CFG_INT_CB("mode", WNCK_PAGER_DISPLAY_CONTENT, CFGF_NONE,
			&pc_pager_parse_mode),
	CFG_BOOL("all_workspaces", TRUE, CFGF_NONE),
	CFG_INT("rows", 1, CFGF_NONE),
	CFG_END()
};

static void pc_pager_print_help()
{
	printf(
"# the pager widget displays the workspaces or the name of the active one\n"
"pager mypager\n"
"{\n"
"    # The pager's mode, set to 'name' or 'content'\n"
"    mode = content\n"
"\n"
"    # set to 'true' to display all workspaces, to 'false' to display only\n"
"    # the active one\n"
"    all_workspaces = true\n"
"\n"
"    # sets the number of rows the pager is organized in\n"
"    rows = 1\n"
"}\n"
"\n"
	);
}

static const PcWidgetInfo pc_pager_info = {
	.name = "pager",
	.instantiate = &pc_pager_instantiate,
	.print_help = &pc_pager_print_help,
	.options = pc_pager_options
};

gboolean pc_module_init(const PcModuleCallbacks* callbacks)
{
	callbacks->register_widget(&pc_pager_info);
	return TRUE;
}

