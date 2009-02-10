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

#ifndef PC_COMMANDLINE_H
#define PC_COMMANDLINE_H

#include <gtk/gtk.h>

/**
 * @brief Holds the commandline-options set by the user
 **/
typedef struct PcCommandlineOpts
{
	//! Path to the configuration, if specified by user, NULL otherwise
	const gchar* config_path;

	//! Path to the modules, if specified by user, NULL otherwise
	const gchar* module_path;
} PcCommandlineOpts;

/* Holds the program's name as in argv[0].
 * It's only valid after pc_commandline_parse has been called */
extern const gchar* pc_program_invocation_name;

/**
 * @brief parses the commandline-arguments
 *
 * @param argc pointer to argc
 * @param argv pointer to argv
 * @return PcCommandlineOpts holding the parsed settings or NULL, if an error
 *         occured while parsing
 **/
const PcCommandlineOpts* pc_commandline_parse(int* argc, char*** argv);

#endif


