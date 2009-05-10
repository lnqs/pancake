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

static GModule* pc_modload_open_modfile(const gchar* name)
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
		g_print("%s: module %s not found at any search-location "
				"or not usable\n", pc_program_invocation_name, name);
	}
	
	g_free(fullname);
	return gmod;
}

gboolean pc_modloader_init(const PcCommandlineOpts* cmdline_opts)
{
	modpath_arg = cmdline_opts->module_path;
	return TRUE;
}

gboolean pc_modloader_load_module(const gchar* name)
{
	static const PcModuleCallbacks mod_callbacks = {
		.register_widget = &pc_modloader_register_widget,
		.register_theme = &pc_modloader_register_theme
	};

	GModule* gmod = pc_modload_open_modfile(name);
	if(!gmod)
		return FALSE;

	gboolean (*module_init_func)(const PcModuleCallbacks*);
	if(!g_module_symbol(gmod, "pc_module_init",
			(gpointer*)&module_init_func))
	{
		g_print("%s\n%s: failed to initialize module %s\n",
				g_module_error(), pc_program_invocation_name, name);
		goto error;
	}

	if(!module_init_func)
	{
		g_print("%s: %s isn't a pancake-module\n",
				pc_program_invocation_name, name);
		goto error;
	}

	if(!(*module_init_func)(&mod_callbacks))
	{
		g_print("%s: module %s failed to initialise\n",
				pc_program_invocation_name, name);
		goto error;
	}

	plugins = g_list_append(plugins, gmod);

	return TRUE;

error:
	if(gmod && !g_module_close(gmod))
		g_print("%s: failed to close module %s\n",
				pc_program_invocation_name, name);
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

