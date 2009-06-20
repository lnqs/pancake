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
#include <errno.h>
#include <string.h>
#include <confuse.h>
#include <pc_misc.h>
#include <pc_module.h>
#include <pc_style.h>
#include "pc_modloader.h"

static gint pc_configparser_validate_height(Config* cfg, ConfigOption* opt)
{
	gint val = cfg_opt_getnint(opt, 0);
	if(val < 0)
	{
		cfg_error(cfg, "height must be at least 0");
        return -1;
	}

	return 0;
}

static gint pc_configparser_validate_width(Config* cfg, ConfigOption* opt)
{
	gfloat val = cfg_opt_getnfloat(opt, 0);
	if(val < 0.0f)
	{
		cfg_error(cfg, "width must be a positiv number");
        return -1;
	}

	return 0;
}

static int pc_configparser_parse_align(
		Config* cfg, ConfigOption* opt, const char* value, void* result)
{
	if(!g_strcmp0(value, "top-left"))
		*(glong*)result = PC_ALIGN_TOP_LEFT;
	else if(!g_strcmp0(value, "top"))
		*(glong*)result = PC_ALIGN_TOP;
	else if(!g_strcmp0(value, "top-right"))
		*(glong*)result = PC_ALIGN_TOP_RIGHT;
	else if(!g_strcmp0(value, "bottom-left"))
		*(glong*)result = PC_ALIGN_BOTTOM_LEFT;
	else if(!g_strcmp0(value, "bottom"))
		*(glong*)result = PC_ALIGN_BOTTOM;
	else if(!g_strcmp0(value, "bottom-right"))
		*(glong*)result = PC_ALIGN_BOTTOM_RIGHT;
	else
	{
		cfg_error(cfg, "invalid value for option %s: %s", opt->name, value);
		return -1;
	}
	
	return 0;
}

static gchar* pc_configparser_determine_filename(
		const PcCommandlineOpts* cmdline_opts)
{
	if(cmdline_opts->config_path)
		return g_strdup(cmdline_opts->config_path);
	else
		return g_strjoin(G_DIR_SEPARATOR_S,
				g_get_home_dir(), ".pancakerc", NULL);
}

static gboolean pc_configparser_load_modules(const gchar* filename)
{
	/* This sucks a bit, since the error-checking of libconfuse forces us
	   to pre-parse the config, but I don't see a better way */
	static ConfigOption firstpass_opts[] = {
		CFG_STR_LIST("modules", "", CFGF_NONE),
		CFG_END()
	};

	gchar buf[1024];
	gchar* modulesline = NULL;
	FILE* file = fopen(filename, "r");
	if(!file)
	{
		g_critical("failed to open %s: %s", filename, strerror(errno));
		return FALSE;
	}

	while(fgets(buf, sizeof(buf), file))
	{
		if(!strncmp("modules", buf, strlen("modules")))
		{
			modulesline = g_strdup(buf);
			break;
		}
	}

	if(!modulesline)
		modulesline = g_strdup("");

	fclose(file);

	/* pass it to confuse */
	Config* cfg = cfg_init(firstpass_opts, CFGF_NOCASE);
	int ret = cfg_parse_buf(cfg, modulesline);
	if(ret == CFG_FILE_ERROR) /* unknown options will be ignored this pass */
	{
		g_critical("failed to open %s: %s", filename, strerror(errno));
		goto error;
	}

	for(int i = 0; i < cfg_size(cfg, "modules"); i++)
	{
		if(!pc_modloader_load_module(cfg_getnstr(cfg, "modules", i), FALSE))
			goto error;
	}

	cfg_free(cfg);
	g_free(modulesline);

	return TRUE;

error:
	cfg_free(cfg);
	g_free(modulesline);
	return FALSE;
}

static ConfigOption* pc_configparser_build_optlist()
{
	ConfigOption* opts = g_new(ConfigOption, pc_modloader_get_num_widgets() +
			pc_modloader_get_num_themes() + 8);

	gint i = 0;
	opts[i++] = (ConfigOption)CFG_STR_LIST("modules", "", CFGF_NONE);
	opts[i++] = (ConfigOption)CFG_STR("theme", NULL, CFGF_NONE);
	opts[i++] = (ConfigOption)CFG_STR_LIST("widgets", "", CFGF_NONE);

	opts[i++] = (ConfigOption)CFG_INT("height",
			PC_PANEL_DEFAULT_HEIGHT, CFGF_NONE);
	opts[i++] = (ConfigOption)CFG_FLOAT("width",
			PC_PANEL_DEFAULT_WIDTH, CFGF_NONE);
	opts[i++] = (ConfigOption)CFG_INT_CB("align",
			PC_PANEL_DEFAULT_ALIGN, CFGF_NONE,
			&pc_configparser_parse_align);
	opts[i++] = (ConfigOption)CFG_BOOL("strut",
			PC_PANEL_DEFAULT_STRUT_ENABLED, CFGF_NONE);

	const GList* cur = pc_modloader_get_widgets();
	while(cur)
	{
		PcWidgetInfo* info = cur->data;
		opts[i++] = (ConfigOption)CFG_SEC((gchar*)info->name,
				info->options, CFGF_MULTI | CFGF_TITLE);
		cur = g_list_next(cur);
	}

	cur = pc_modloader_get_themes();
	while(cur)
	{
		PcWidgetInfo* info = cur->data;
		opts[i++] = (ConfigOption)CFG_SEC((gchar*)info->name,
				info->options, CFGF_MULTI | CFGF_TITLE);
		cur = g_list_next(cur);
	}

	opts[i++] = (ConfigOption)CFG_END();

	return opts;
}

static gboolean pc_configparser_load_theme(Config* config)
{
	const gchar* theme = cfg_getstr(config, "theme");
	if(!theme)
		return TRUE;

	Config* section = NULL;
	PcThemeInfo* info = NULL;

	/* find the section for our active theme */
	const GList* cur = pc_modloader_get_themes();
	while(cur)
	{
		info = cur->data;
		for(gint i = 0; i < cfg_size(config, info->name); i++)
		{
			section = cfg_getnsec(config, info->name, i);

			if(!g_strcmp0(cfg_title(section), theme))
				goto found;
		}

		cur = g_list_next(cur);
	}

found:
	if(!section || g_strcmp0(cfg_title(section), theme))
	{
		g_critical("theme %s isn't defined", theme);
		return FALSE;
	}

	/* instantiate it */
	GtkStyle* style = info->instantiate(section);
	if(!style)
	{
		g_critical("failed to instantiate theme %s", info->name);
		return FALSE;
	}

	pc_theme = style;

	return TRUE;
}

static gboolean pc_configparser_load_widgets(Config* config, PcPanel* panel)
{
	for(int i = 0; i < cfg_size(config, "widgets"); i++)
	{
		const gchar* widget = cfg_getnstr(config, "widgets", i);
		gboolean expand = FALSE;
		Config* section = NULL;
		PcWidgetInfo* info = NULL;

		/* options given for this widget? */
		if(!strncmp(widget, "expand:", strlen("expand")))
		{
			expand = TRUE;
			widget += strlen("expand:");
		}

		/* find the section for our current widget */
		const GList* cur = pc_modloader_get_widgets();
		while(cur)
		{
			info = cur->data;
			for(gint j = 0; j < cfg_size(config, info->name); j++)
			{
				section = cfg_getnsec(config, info->name, j);

				if(!g_strcmp0(cfg_title(section), widget))
					goto found;
			}

			cur = g_list_next(cur);
		}

found:
		if(!section || g_strcmp0(cfg_title(section), widget))
		{
			g_critical("widget %s isn't defined", widget);
			return FALSE;
		}

		/* We've got it, let's create it */
		GtkWidget* w = info->instantiate(section);
		if(!w)
		{
			g_critical("failed to create widget %s\n", info->name);
			return FALSE;
		}

		gtk_box_pack_start(pc_panel_get_box(panel),
				w, expand, expand, pc_style_get_widget_padding(pc_theme));
	}
	
	return TRUE;
}

gboolean pc_configparser_parse(
		PcPanel* panel, const PcCommandlineOpts* cmdline_opts)
{
	gchar* filename = pc_configparser_determine_filename(cmdline_opts);
	
	/* first pass -- just load the modules. */
	if(!pc_configparser_load_modules(filename))
	{
		g_free(filename);
		return FALSE;
	}

	/* time to do the real parsing */
	ConfigOption* opts = pc_configparser_build_optlist();
		
	Config* cfg = cfg_init(opts, CFGF_NOCASE);

	cfg_set_validate_func(cfg, "height", &pc_configparser_validate_height);
	cfg_set_validate_func(cfg, "width", &pc_configparser_validate_width);

	gint ret = cfg_parse(cfg, filename);
	if(ret == CFG_FILE_ERROR)
	{
		g_critical("failed to open %s: %s\n", filename, strerror(errno));
		goto error_opts;
	}
	else if(ret == CFG_PARSE_ERROR)
		goto error_opts;

	if(!pc_configparser_load_theme(cfg))
		goto error_config;

	if(!pc_configparser_load_widgets(cfg, panel))
		goto error_config;

	pc_panel_set_width(panel, cfg_getfloat(cfg, "width"));
	pc_panel_set_height(panel, cfg_getint(cfg, "height"));
	pc_panel_set_align(panel, cfg_getint(cfg, "align"));
	pc_panel_set_strut_enabled(panel, cfg_getbool(cfg, "strut"));

	cfg_free(cfg);
	g_free(opts);
	g_free(filename);

	return TRUE;

error_config:
	cfg_free(cfg);

error_opts:
	g_free(opts);
	g_free(filename);

	return FALSE;
}

