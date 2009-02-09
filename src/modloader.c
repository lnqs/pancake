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

#include "modloader.h"
#include "commandline.h"

#ifndef MODULE_PATH
#error "MODULE_PATH isn't defined!"
#endif

static GList* plugins = 0;
static PancakeModule* theme = 0;
static const gchar* modpath_arg = 0;

gboolean pc_modloader_init(const PcCommandlineOpts* cmdline_opts)
{
	modpath_arg = cmdline_opts->module_path;
	return TRUE;
}

PancakeModule* pc_modloader_load(PancakeModuleType type, const gchar* name)
{
	if(type == PC_MODTYPE_PLUGIN && theme)
	{
		g_print("%s: attempt to open second theme-module, but only one "
				"may be used\n", pc_program_invocation_name);
		return NULL;
	}

	GModule* gmod = NULL;
	
	if(modpath_arg)
	{
		gchar* path = g_module_build_path(modpath_arg, name);
		gmod = g_module_open(path, 0);
		g_free(path);
	}

	if(!gmod)
	{
		gchar* path1 = g_strconcat(g_get_home_dir(), "/.pancake/modules", NULL);
		gchar* path2 = g_module_build_path(path1, name);
		gmod = g_module_open(path2, 0);
		g_free(path1);
		g_free(path2);
	}

	if(!gmod)
	{
		gchar* path = g_module_build_path(MODULE_PATH, name);
		gmod = g_module_open(path, 0);
		g_free(path);
	}

	if(!gmod)
	{
		g_print("%s: module %s not found at any search-location\n",
				pc_program_invocation_name, name);
		return NULL;
	}

	PancakeModule* modinfo;
	if(!g_module_symbol(gmod, "pancake_modinfo", (gpointer*)&modinfo))
	{
		g_print("%s: module %s isn't a pancake-module\n",
				pc_program_invocation_name, name);
		if(!g_module_close(gmod))
			g_print("%s: failed to close module %s\n",
					pc_program_invocation_name, name);
		return NULL;
	}

	if(modinfo->type != type)
	{
		g_print("%s: module %s is not of the expected type\n",
				pc_program_invocation_name, name);
		if(!g_module_close(gmod))
			g_print("%s: failed to close module %s\n",
					pc_program_invocation_name, name);
		return NULL;
	}

	if(!modinfo->init())
	{
		g_print("%s: initialisation of module %s failed\n",
				pc_program_invocation_name, name);
		if(!g_module_close(gmod))
			g_print("%s: failed to close module %s\n",
					pc_program_invocation_name, name);
		return NULL;
	}
	
	if(type == PC_MODTYPE_THEME)
		theme = modinfo;
	else
		plugins = g_list_append(plugins, modinfo);

	modinfo->gmodule = gmod;

	return modinfo;
}

void pc_modloader_cleanup()
{
	GList* cur = plugins;
	while(cur)
	{
		PancakeModule* d = cur->data;
		d->fini();
		g_module_close(d->gmodule);

		GList* next = g_list_next(cur);
		plugins = g_list_delete_link(plugins, cur);
		cur = next;
	}

	if(theme)
	{
		theme->fini();
		g_module_close(theme->gmodule);
		theme = 0;
	}
}

