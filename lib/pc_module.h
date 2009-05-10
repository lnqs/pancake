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
#include <confuse.h>

/**
 * @brief struct providing information about a widget provided by a module
 *
 * structs of this type are given to the application by modules with the
 * register_widget-callback to register new widgets.
 **/
typedef struct PcWidgetInfo
{
	const gchar* name; //!< unique name of the widget-type

	//! pointer to a function suitable to create an instance of the widget
	GtkWidget* (*instantiate)(cfg_t* config);

	//! libconfuse-conform options that may be set for the widget
	cfg_opt_t* options;
} PcWidgetInfo;

/**
 * @brief struct providing information about a theme provided by a module
 *
 * structs of this type are given to the application by modules with the
 * register_theme-callback to register new themes.
 **/
typedef struct PcThemeInfo
{
	const gchar* name; //!< unique name of the theme-type

	//! pointer to a function suitable to create an instance of the
	//! theme's style
	GtkStyle* (*instantiate)(cfg_t* config);

	//! libconfuse-conform options that may be set for the theme
	cfg_opt_t* options;
} PcThemeInfo;

/**
 * @brief struct of callbacks given to modules
 *
 * when a module is initialized, an instance of this struct is given to it,
 * to provide the possibility to call-back the application to register widgets
 * and themes
 **/
typedef struct PcModuleCallbacks
{
	void (*register_widget)(const PcWidgetInfo*);
	void (*register_theme)(const PcThemeInfo*);
} PcModuleCallbacks;

