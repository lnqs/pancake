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

#ifndef PC_MODLOADER_H
#define PC_MODLOADER_H

#include <gtk/gtk.h>
#include <pc_module.h>
#include "pc_commandline.h"
#include "pc_panel.h"

/**
 * @brief Initialises the module-loader
 *
 * @param cmdline_opts The options parse from commandline
 * @return TRUE on success, FALSE otherwise
 **/
gboolean pc_modloader_init(const PcCommandlineOpts* cmdline_opts);

/**
 * @brief Loads a plugin 
 *
 * @param name the name of the module to load
 * @return the newly loaded module, or NULL if an error occured
 **/
PancakePlugin* pc_modloader_load_plugin(const gchar* name);

/**
 * @brief Loads a theme 
 *
 * @param name the name of the module to load
 * @return the newly loaded module, or NULL if an error occured
 **/
PancakeTheme* pc_modloader_load_theme(const gchar* name);

/**
 * @brief Returns a plugin by it's name
 *
 * @param name the name of the module to search for
 * @return pointer to the module or NULL if no such module exists
 **/
PancakePlugin* pc_modloader_get_plugin(const gchar* name);

/**
 * @brief Returns the theme-module
 *
 * @return the theme-module
 **/
PancakeTheme* pc_modloader_get_theme();

/**
 * @brief Cleans up the module-subsystem
 **/
void pc_modloader_cleanup();

#endif

