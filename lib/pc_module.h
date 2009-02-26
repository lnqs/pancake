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

#ifndef PC_MODULE_H
#define PC_MODULE_H

#include <gtk/gtk.h>
#include "pc_style.h"

/**
 * @brief Macro to define a interface in plugins usable by pancake
 **/
#define PANCAKE_PLUGIN(modinfo) \
		PancakePlugin* _pc_mod_get_plugininfo() { return &modinfo; }

/**
 * @brief Macro to define a interface in themes usable by pancake
 **/
#define PANCAKE_THEME(modinfo) \
		PancakeTheme* _pc_mod_get_themeinfo() { return &modinfo; }

/**
 * @brief Plugin providing new widgets for the panel
 **/
typedef struct PancakePlugin
{
	GModule* gmodule; //!< set by pancake. Ignore it from within modules.

	//! Has to be set to a unique name to identify the plugin
	const gchar* name;

	//! called immediatly after loading the plugin -- the plugin can use
	//! this to allocated static ressources shared over multiple instaces,
	//! initialize data, etc.
	//! Has to return TRUE on success or FALSE if an error occured
	gboolean (*init)();

	//! called when the plugin is unloaded to give the plugin the chance
	//! to clean up and deallocate ressources
	void (*fini)();

	//! called when a new instace of the plugin's widget has to be created	
	GtkWidget* (*new_widget)();
} PancakePlugin;

/**
 * @brief The for drawing the panel and widgets within
 **/
typedef struct PancakeTheme
{
	GModule* gmodule; //!< set by pancake. Ignore it from within modules.

	//! Has to be set to a unique name to identify the theme 
	const gchar* name;

	//! called immediatly after loading the theme -- the theme can use
	//! this to allocated static ressources shared over multiple instaces,
	//! initialize data, etc.
	//! Has to return TRUE on success or FALSE if an error occured
	gboolean (*init)();

	//! called when the theme is unloaded to give the plugin the chance
	//! to clean up and deallocate ressources
	void (*fini)();

	//! called by pancake when a new instance of the theme has to be created.
	//! A GtkStyle is fine, but usually you want to return an instance of a
	//! class derived from from PcStyle, since this is able to draw the
	//! panel in a less generic way
	GtkStyle* (*new_style)();
} PancakeTheme;

#endif

