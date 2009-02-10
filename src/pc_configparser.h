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

#ifndef PC_CONFIGPARSER_H
#define PC_CONFIGPARSER_H

#include <gtk/gtk.h>
#include "pc_commandline.h"
#include "pc_types.h"
#include "pc_panel.h"

/**
 * @brief Parses the configuration
 *
 * This function parses the configuration and loads modules specified
 * there. Loaded modules will get their configuration.
 *
 * @return TRUE if parsing the configuration succeeded, false otherwise
 **/
gboolean pc_configparser_parse(
		PcPanel* panel, const PcCommandlineOpts* cmdline_opts);

#endif

