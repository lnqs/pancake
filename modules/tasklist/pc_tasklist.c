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

static GtkWidget* pc_tasklist_instantiate(Config* config)
{
	GtkWidget* tl = wnck_tasklist_new(wnck_screen_get_default());
	wnck_tasklist_set_button_relief(WNCK_TASKLIST(tl), GTK_RELIEF_NONE);
	wnck_tasklist_set_include_all_workspaces(
			WNCK_TASKLIST(tl), cfg_getbool(config, "all_workspaces"));
	wnck_tasklist_set_grouping(
			WNCK_TASKLIST(tl), cfg_getint(config, "grouping"));
	return tl;
}

static int pc_tasklist_parse_grouping(
		Config* cfg, ConfigOption* opt, const char* value, void* result)
{
	if(!g_strcmp0(value, "never"))
		*(glong*)result = WNCK_TASKLIST_NEVER_GROUP;
	else if(!g_strcmp0(value, "auto"))
		*(glong*)result = WNCK_TASKLIST_AUTO_GROUP;
	else if(!g_strcmp0(value, "always"))
		*(glong*)result = WNCK_TASKLIST_ALWAYS_GROUP;
	else
	{
		cfg_error(cfg, "invalid value for option %s: %s", opt->name, value);
		return -1;
	}
	
	return 0;
}

static void pc_tasklist_print_help()
{
	printf("TODO\n");
}

static ConfigOption pc_tasklist_options[] = {
	CFG_BOOL("all_workspaces", TRUE, CFGF_NONE),
	CFG_INT_CB("grouping", WNCK_TASKLIST_NEVER_GROUP, CFGF_NONE,
			&pc_tasklist_parse_grouping),
	CFG_END()
};

static const PcWidgetInfo pc_tasklist_info = {
	.name = "tasklist",
	.instantiate = &pc_tasklist_instantiate,
	.print_help = &pc_tasklist_print_help,
	.options = pc_tasklist_options
};

gboolean pc_module_init(const PcModuleCallbacks* callbacks)
{
	callbacks->register_widget(&pc_tasklist_info);
	return TRUE;
}

