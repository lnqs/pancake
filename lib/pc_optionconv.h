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

#ifndef PC_OPTIONCONV_H
#define PC_OPTIONCONV_H

#include <gtk/gtk.h>
#include "pc_types.h"

/**
 * @brief parses a PcAlignment from a string
 *
 * @param value the string to parse from
 * @param target the parsed value will be written here
 * @return TRUE on success, FALSE otherwise
 **/
gboolean pc_optionconv_alignment(const gchar* value, PcAlignment* target);

/**
 * @brief parses a float from a string
 *
 * @param value the string to parse from
 * @param target the parsed value will be written here
 * @return TRUE on success, FALSE otherwise
 **/
gboolean pc_optionconv_float(const gchar* value, gfloat* target);

/**
 * @brief parses an int from a string
 *
 * @param value the string to parse from
 * @param target the parsed value will be written here
 * @return TRUE on success, FALSE otherwise
 **/
gboolean pc_optionconv_int(const gchar* value, gint* target);

/**
 * @brief parses a bool from a string
 *
 * @param value the string to parse from
 * @param target the parsed value will be written here
 * @return TRUE on success, FALSE otherwise
 **/
gboolean pc_optionconv_bool(const gchar* value, gboolean* target);

#endif

