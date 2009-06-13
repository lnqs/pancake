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
 * @brief Loads a module
 *
 * @param name the name of the module to load
 * @param silent if set to TRUE, messages about errors are supressed
 * @return TRUE on success, FALSE otherwise
 **/
gboolean pc_modloader_load_module(const gchar* name, gboolean silent);

/**
 * @brief Cleans up the module-subsystem
 **/
void pc_modloader_cleanup();

/**
 * @brief Returns the total number of registered widget-"providers"
 *
 * this functions returns the number of different widgets registered
 * by modules. These are not widgets acually created, but widgets that
 * can be instantiated any time.
 *
 * @return the number of instantiable widgets
 **/
gint pc_modloader_get_num_widgets();

/**
 * @brief Returns the total number of registered theme-"providers"
 *
 * this functions returns the number of different themes registered
 * by modules. These are not themes acually created, but themes that
 * can be instantiated any time.
 *
 * @return the number of instantiable theme
 **/
gint pc_modloader_get_num_themes();

/**
 * @brief Returns the list of widgets
 *
 * this function returns the internal list of instantiable widget-information
 *
 * @return the list of instantiable widgets
 **/
const GList* pc_modloader_get_widgets();

/**
 * @brief Returns the list of themes
 *
 * this function returns the internal list of instantiable theme-information
 *
 * @return the list of instantiable themes
 **/
const GList* pc_modloader_get_themes();

/**
 * @brief prints configurationfile-template to stdout
 **/
void pc_modloader_print_modulehelp();

#endif

