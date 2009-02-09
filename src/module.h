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

#ifndef MODULE_H
#define MODULE_H

/**
 * @todo docs
 **/
typedef enum PancakeModuleType
{
	PC_MODTYPE_THEME,
	PC_MODTYPE_PLUGIN
} PancakeModuleType;

/**
 * @todo docs
 **/
typedef struct PancakeModule
{
	GModule* gmodule;
	PancakeModuleType type;
	gboolean (*init)();
	void (*fini)();
	gboolean (*set_option)(const gchar*, const gchar*);
} PancakeModule;

/**
 * @todo docs
 **/
typedef struct PancakeTheme
{
	PancakeModule mod;
} PancakeTheme;

/**
 * @todo docs
 **/
typedef struct PancakePlugin
{
	PancakeModule mod;
} PancakePlugin;

#endif

