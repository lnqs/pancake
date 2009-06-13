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

#include "pc_modloader.h"
#include <libgen.h>
#include <string.h>
#include <pc_module.h>
#include "pc_commandline.h"

static GList* plugins = NULL;
static const gchar* modpath_arg = NULL;

static GList* widgets = NULL;
static GList* themes = NULL;

static void pc_modloader_register_widget(const PcWidgetInfo* info)
{
	widgets = g_list_append(widgets, (PcWidgetInfo*)info);
}

static void pc_modloader_register_theme(const PcThemeInfo* info)
{
	themes = g_list_append(themes, (PcThemeInfo*)info);
}

static GModule* pc_modload_open_modfile(const gchar* name, gboolean silent)
{
	GModule* gmod = NULL;
	gchar* fullname = g_strconcat("lib", name, NULL);
	
	if(modpath_arg)
	{
		gchar* path = g_module_build_path(modpath_arg, fullname);
		gmod = g_module_open(path, 0);
		g_free(path);
	}

	if(!gmod)
	{
		gchar* path1 = g_strconcat(g_get_home_dir(), "/.pancake/modules", NULL);
		gchar* path2 = g_module_build_path(path1, fullname);
		gmod = g_module_open(path2, 0);
		g_free(path1);
		g_free(path2);
	}

	if(!gmod)
	{
		gchar* path = g_module_build_path(PC_MODULE_PATH, fullname);
		gmod = g_module_open(path, 0);
		g_free(path);
	}

	if(!gmod)
	{
		if(!silent)
			g_critical("module %s not found at any search-location or "
					"not usable", name);
	}
	
	g_free(fullname);
	return gmod;
}

gboolean pc_modloader_init(const PcCommandlineOpts* cmdline_opts)
{
	modpath_arg = cmdline_opts->module_path;
	return TRUE;
}

gboolean pc_modloader_load_module(const gchar* name, gboolean silent)
{
	static const PcModuleCallbacks mod_callbacks = {
		.register_widget = &pc_modloader_register_widget,
		.register_theme = &pc_modloader_register_theme
	};

	GModule* gmod = pc_modload_open_modfile(name, silent);
	if(!gmod)
		return FALSE;

	gboolean (*module_init_func)(const PcModuleCallbacks*);
	if(!g_module_symbol(gmod, "pc_module_init",
			(gpointer*)&module_init_func))
	{
		if(!silent)
			g_critical("%s\nfailed to initialize module %s",
					g_module_error(), name);
		goto error;
	}

	if(!module_init_func)
	{
		if(!silent)
			g_critical("%s isn't a pancake-module", name);
		goto error;
	}

	if(!(*module_init_func)(&mod_callbacks))
	{
		if(!silent)
			g_critical("module %s failed to initialise", name);
		goto error;
	}

	plugins = g_list_append(plugins, gmod);

	return TRUE;

error:
	if(gmod && !g_module_close(gmod))
		if(!silent)
			g_critical("failed to close module %s", name);
	return FALSE;
}

void pc_modloader_cleanup()
{
	GList* cur = plugins;
	while(cur)
	{
		GModule* gmod = cur->data;

		void (*module_fini_func)();
		if(g_module_symbol(gmod, "pc_module_fini",
				(gpointer*)&module_fini_func))
		{
			(*module_fini_func)();
		}

		g_module_close(gmod);

		GList* next = g_list_next(cur);
		plugins = g_list_delete_link(plugins, cur);
		cur = next;
	}
}

gint pc_modloader_get_num_widgets()
{
	gint num = 0;
	GList* cur = widgets;
	while(cur)
	{
		num++;
		cur = g_list_next(cur);
	}

	return num;
}

gint pc_modloader_get_num_themes()
{
	gint num = 0;
	GList* cur = themes;
	while(cur)
	{
		num++;
		cur = g_list_next(cur);
	}
	
	return num;
}

const GList* pc_modloader_get_widgets()
{
	return widgets;
}

const GList* pc_modloader_get_themes()
{
	return themes;
}

static gboolean pc_modloader_add_files_to_list(GList** list, const gchar* path)
{
	GDir* dir = g_dir_open(path, 0, NULL);
	if(!dir)
		return FALSE;
	
	const gchar* filename;
	while((filename = g_dir_read_name(dir)))
	{
		/* Regular file? */
		gchar* fullname = g_strconcat(path, "/", filename, NULL);
		gboolean regfile = g_file_test(fullname, G_FILE_TEST_IS_REGULAR) &&
				!g_file_test(fullname, G_FILE_TEST_IS_SYMLINK);
		g_free(fullname);
		if(!regfile)
			continue;

		/* strip leading lib and remove extensions */
		gchar* modname = g_strdup(filename);
		if(g_str_has_prefix(modname, "lib"))
		{
			gchar* tmp = g_strdup(modname + 3);
			g_free(modname);
			modname = tmp;
		}

		gchar* tmp = modname + strlen(modname);
		while(tmp != modname && *tmp != '.')
			tmp--;
		if(tmp != modname)
			*tmp = '\0';

		/* avoid dublications */
		gboolean isdup = FALSE;
		GList* cur = *list;
		while(cur)
		{
			if(!g_strcmp0(cur->data, modname))
			{
				isdup = TRUE;
				break;
			}
			cur = g_list_next(cur);
		}

		if(isdup)
			g_free(modname);
		else
			*list = g_list_append(*list, modname);
	}
	
	g_dir_close(dir);
	
	return TRUE;
}

void pc_modloader_print_modulehelp()
{
	GList* files = NULL;

	if(modpath_arg)
	{
		if(!pc_modloader_add_files_to_list(&files, modpath_arg))
		{
			g_critical("Failed to open %s", modpath_arg);
			return;
		}
	}

	gchar* usrmoddir = g_strconcat(g_get_home_dir(), "/.pancake/modules", NULL);
	pc_modloader_add_files_to_list(&files, usrmoddir);
	g_free(usrmoddir);

	pc_modloader_add_files_to_list(&files, PC_MODULE_PATH);

	printf(
"# pancake configuration\n"
"# This file is parsed by pancake at startup and may be named\n"
"# $(HOME)/.pancakerc (or lay in any other location and be specified with\n"
"# the --config-option).\n"
"# \n"
"# In this template, a description for every option of all installed modules\n"
"# is included.\n"
"#\n"
"# Comments start with # or // or may be enclosed in /* and */"
"\n\n"
	);

	printf(
"# The following line specifies which modules to load. In this example, all\n"
"# found modules are included, but you can remove the ones you don't need\n"
"# Module can provide two types of extensions: Themes to change the look of\n"
"# the panel and it's widgets, and new widgets that can be added to the panel\n"
	);

	printf("modules = { ");
	GList* cur = files;
	gboolean first = TRUE;
	while(cur)
	{
		if(pc_modloader_load_module(cur->data, TRUE))
		{
			if(!first)
				printf(", ");
			else
				first = FALSE;

			printf("%s", (gchar*)cur->data);
		}
		cur = g_list_next(cur);
	}
	printf(" }\n\n");

	printf(
"# this sets the panels height in pixels\n"
"height = 24\n"
"\n"
"# the width of the panel. It can be set in two ways: as number in range\n"
"# from 0.0 to 1.0 to specify the panel's width relativ to the screen's\n"
"# width, or, if a number greater than 1 is given, it will be used as amount\n"
"# of pixels the panel will have in width.\n"
"width = 0.98\n"
"\n"
"# the panels align on the screen -- may be set to 'top', 'bottom',\n"
"# 'top_left', 'top_right', 'bottom_left' or 'bottom_right'\n"
"align = bottom\n"
"\n"
"# may be set to 'true' or 'false' to enable/disable strut.\n"
"# An enabled strut will prevent maximized windows from hiding the panel\n"
"strut = true\n"
"\n"
	);

	printf(
"# Pancake support theme-engines, and the next part of the configuration\n"
"# cares about their settings.\n"
"# Usually, the engine itself defines the basic look of the panel and it's\n"
"# widgets, while it support's various options to set change things like\n"
"# colors.\n"
"# Only one theme can be active at time. You may have as much theme-blocks as\n"
"# you want, but only the one specified at the end of this file will be used.\n"
"#\n"
"# A block has following syntax:\n"
"#\n"
"# THEMENAME IDENTIFIER\n"
"# {\n"
"#     [OPTION = VALUE]\n"
"#     [...]\n"
"# }\n"
"#\n"
"# where THEMENAME is the name of the theme as defined in the module\n"
"# providing the theme, and IDENTIFIER is a arbitrary name you give to your\n"
"# theme-configuration to be able to reference to it later.\n"
"\n"
"# The following lines contains all themes found by pancake. You can remove\n"
"# those you don't want and change it's options\n"
	);

	cur = themes;
	while(cur)
	{
		((PcThemeInfo*)(cur->data))->print_help();
		cur = g_list_next(cur);
	}

	printf(
"# Next, we'll configure widgets. A widget-block has the same semantics as a\n"
"# theme-block. You also can have as much of them as you want and have a\n"
"# widget used multiple times.\n"
"# By writing a widget-block itself, the widget isn't shown already -- you\n"
"# enable it at the end of this file\n"
"\n"
"# Again, a list of all found widgets follows.\n"
	);

	cur = widgets;
	while(cur)
	{
		((PcWidgetInfo*)(cur->data))->print_help();
		cur = g_list_next(cur);
	}

	printf(
"# The next line sets the theme to use -- you have to give the identifier\n"
"# of the one you want to be active, or comment out the entire line to\n"
"# disable the use of a theme and have the panel in the look of the active\n"
"# GTK-theme\n"
"theme = mytheme\n"
"\n"
	);

	printf(
"# The next (and last) line set's the widgets to use. They'll be displayed\n"
"# in the order you specifiy them here by their identifier.\n"
"# You can preprend the string 'expand:' to one of them, to have this widget\n"
"# fill the space left by the others.\n"
"widgets = { expand:mytasklist, myclock }\n"
"\n"
);

	cur = files;
	while(cur)
	{
		g_free(cur->data);

		GList* next = g_list_next(cur);
        plugins = g_list_delete_link(plugins, cur);
        cur = next;
	}
}

