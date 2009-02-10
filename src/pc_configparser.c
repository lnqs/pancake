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
#include "pc_module.h"
#include "pc_modloader.h"

static GKeyFile* pc_configparser_open_file_by_name(const gchar* name)
{
	GError* error = 0;
	GKeyFile* file = g_key_file_new();

	g_key_file_load_from_file(file, name, 0, &error);
	if(error)
	{
		g_print("%s: couldn't open configurationfile: %s\n",
				pc_program_invocation_name, error->message);
		g_key_file_free(file);
		g_error_free(error);
		return NULL;
	}

	return file;
}

static GKeyFile* pc_configparser_open_file_by_defaultpaths()
{
	GError* error = 0;
	GKeyFile* file = g_key_file_new();

	gchar* homepath = g_strconcat(g_get_home_dir(), "/.pancake", NULL);
	const gchar* dirs[] = { homepath, "/etc", NULL };

	g_key_file_load_from_dirs(file, "pancakerc", dirs, NULL, 0, &error);
	if(error)
	{
		g_print("%s: couldn't open configurationfile: %s\n",
				pc_program_invocation_name, error->message);
		g_key_file_free(file);
		g_free(homepath);
		g_error_free(error);
		return NULL;
	}

	g_free(homepath);
	return file;
}

static GKeyFile* pc_configparser_open_file(
		const PcCommandlineOpts* cmdline_opts)
{
	if(cmdline_opts->config_path)
		return pc_configparser_open_file_by_name(cmdline_opts->config_path);
	else
		return pc_configparser_open_file_by_defaultpaths();
}

gboolean pc_configparser_parse(
		PcPanel* panel, const PcCommandlineOpts* cmdline_opts)
{
	GKeyFile* file = pc_configparser_open_file(cmdline_opts);
	if(!file)
		return FALSE;

	PancakeModule* module = 0;

	gsize num_groups;
	gchar** groups = g_key_file_get_groups(file, &num_groups);
	for(gsize i = 0; i < num_groups; i++)
	{
		if(!g_strcmp0(groups[i], "theme"))
		{
			GError* error = 0;
			gchar* value = g_key_file_get_value(file, "theme", "name", &error);
			if(error)
			{
				g_print("%s: %s\n",	pc_program_invocation_name, error->message);
				g_error_free(error);
				g_strfreev(groups);
				g_key_file_free(file);
				return FALSE;
			}
			module = pc_modloader_load(PC_MODTYPE_THEME, value);
			if(!module)
			{
				g_strfreev(groups);
				g_key_file_free(file);
				g_free(value);
				return FALSE;
			}
			g_free(value);
		}
		else if(g_strcmp0(groups[i], "panel"))
		{
			module = pc_modloader_load(PC_MODTYPE_PLUGIN, groups[i]);
			if(!module)
			{
				g_strfreev(groups);
				g_key_file_free(file);
				return FALSE;
			}
		}

		gsize num_keys;
		gchar** keys = g_key_file_get_keys(file, groups[i], &num_keys, NULL);
		for(gsize j = 0; j < num_keys; j++)
		{
			if(!g_strcmp0(groups[i], "theme") && !g_strcmp0(keys[j], "name"))
				continue;

			gchar* value = g_key_file_get_value(file, groups[i], keys[j], NULL);
			if(!(g_strcmp0(groups[i], "panel") ?
					module->set_option(keys[j], value)
					: pc_panel_set_option(panel, keys[j], value)))
			{
				g_strfreev(groups);
				g_strfreev(keys);
				g_key_file_free(file);
				return FALSE;
			}
			g_free(value);
		}
		g_strfreev(keys);
	}
	g_strfreev(groups);

	g_key_file_free(file);
	return TRUE;
}

