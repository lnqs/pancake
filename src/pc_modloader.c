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
#include "pc_commandline.h"

static GList* plugins = NULL;
static PancakeTheme* theme = NULL;
static const gchar* modpath_arg = NULL;

static GModule* pc_modload_open_modfile(const gchar* name)
{
	GModule* gmod = NULL;
	gchar* fullname = g_strconcat("lib", name, NULL);
	
	if(modpath_arg)
	{
		gchar* path = g_module_build_path(modpath_arg, fullname);
		gmod = g_module_open(path, 0);

		if(!gmod)
			g_print("%s: couldn't open module %s at %s: %s\n",
					pc_program_invocation_name, name, path, g_module_error());

		g_free(path);
	}

	if(!gmod)
	{
		gchar* path1 = g_strconcat(g_get_home_dir(), "/.pancake/modules", NULL);
		gchar* path2 = g_module_build_path(path1, fullname);
		gmod = g_module_open(path2, 0);

		if(!gmod)
			g_print("%s: couldn't open module %s at %s: %s\n",
					pc_program_invocation_name, name, path2, g_module_error());

		g_free(path1);
		g_free(path2);
	}

	if(!gmod)
	{
		gchar* path = g_module_build_path(PC_MODULE_PATH, fullname);
		gmod = g_module_open(path, 0);

		if(!gmod)
			g_print("%s: couldn't open module %s at %s: %s\n",
					pc_program_invocation_name, name, path, g_module_error());

		g_free(path);
	}

	if(!gmod)
	{
		g_print("%s: module %s not found at any search-location\n",
				pc_program_invocation_name, name);
	}
	
	g_free(fullname);
	return gmod;
}

gboolean pc_modloader_init(const PcCommandlineOpts* cmdline_opts)
{
	modpath_arg = cmdline_opts->module_path;
	return TRUE;
}

PancakePlugin* pc_modloader_load_plugin(const gchar* name)
{
	GModule* gmod = pc_modload_open_modfile(name);
	if(!gmod)
		return NULL;

	PancakePlugin* (*get_plugininfo_func)();
	if(!g_module_symbol(gmod, "_pc_mod_get_plugininfo",
			(gpointer*)&get_plugininfo_func))
	{
		g_print("%s\n%s: failed to initialize plugin %s\n",
				g_module_error(), pc_program_invocation_name, name);
		goto error;
	}

	PancakePlugin* modinfo;
	if(!get_plugininfo_func || !(modinfo = (*get_plugininfo_func)()))
	{
		g_print("%s: module %s isn't a pancake-plugin\n",
				pc_program_invocation_name, name);
		goto error;
	}

	if(modinfo->init && !(*modinfo->init)())
	{
		g_print("%s: initialisation of module %s failed\n",
				pc_program_invocation_name, name);
		goto error;
	}
	
	plugins = g_list_append(plugins, modinfo);
	modinfo->gmodule = gmod;

	return modinfo;

error:
	if(gmod && !g_module_close(gmod))
		g_print("%s: failed to close module %s\n",
				pc_program_invocation_name, name);
	return NULL;
}

PancakeTheme* pc_modloader_load_theme(const gchar* name)
{
	if(theme)
	{
		g_print("%s: attempt to load theme %s, but there's already "
				"one registered\n", pc_program_invocation_name, name);
		return NULL;
	}

	GModule* gmod = pc_modload_open_modfile(name);
	if(!gmod)
		return NULL;
	
	PancakeTheme* (*get_themeinfo_func)();
	if(!g_module_symbol(gmod, "_pc_mod_get_themeinfo",
			(gpointer*)&get_themeinfo_func))
	{
		g_print("%s\n%s: failed to initialize theme %s\n",
				g_module_error(), pc_program_invocation_name, name);
		goto error;
	}

	PancakeTheme* modinfo;
	if(!get_themeinfo_func || !(modinfo = (*get_themeinfo_func)()))
	{
		g_print("%s: module %s isn't a pancake-theme\n",
				pc_program_invocation_name, name);
		goto error;
	}

	if(modinfo->init && !(*modinfo->init)())
	{
		g_print("%s: initialisation of module %s failed\n",
				pc_program_invocation_name, name);
		goto error;
	}
	
	theme = modinfo;
	modinfo->gmodule = gmod;

	return modinfo;

error:
	if(gmod && !g_module_close(gmod))
		g_print("%s: failed to close module %s\n",
				pc_program_invocation_name, name);
	return NULL;
}

PancakePlugin* pc_modloader_get_plugin(const gchar* name)
{
	for(GList* cur = plugins; cur; cur = g_list_next(cur))
	{
		PancakePlugin* d = cur->data;

		if(d->name == name)
			return d;
	}

	return NULL;
}

PancakeTheme* pc_modloader_get_theme()
{
	return theme;
}

void pc_modloader_cleanup()
{
	GList* cur = plugins;
	while(cur)
	{
		PancakePlugin* d = cur->data;
		if(d->fini)
			(*d->fini)();
		g_module_close(d->gmodule);

		GList* next = g_list_next(cur);
		plugins = g_list_delete_link(plugins, cur);
		cur = next;
	}

	if(theme)
	{
		if(theme->fini)
			(*theme->fini)();
		g_module_close(theme->gmodule);
		theme = NULL;
	}
}

