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

#ifndef PC_TYPES_H
#define PC_TYPES_H

#include <confuse.h>

/**
 * @brief enum to display variuos alignments on screen
 **/
typedef enum PcAlignment
{
	PC_ALIGN_0,
	PC_ALIGN_TOP_LEFT,
	PC_ALIGN_TOP,
	PC_ALIGN_TOP_RIGHT,
	PC_ALIGN_BOTTOM_LEFT,
	PC_ALIGN_BOTTOM,
	PC_ALIGN_BOTTOM_RIGHT,
	PC_ALIGN_LAST
} PcAlignment;

/**
 * @brief typedef of libconfuse's cfg_t, just to have a consistent
 *        naming-scheme
 **/
typedef cfg_t Config;

/**
 * @brief typedef of libconfuse's cfg_opt_t, just to have a consistent
 *        naming-scheme
 **/
typedef cfg_opt_t ConfigOption;

/**
 * @brief Color-datatype
 **/
typedef struct PcColor
{
	gfloat r;
	gfloat g;
	gfloat b;
} PcColor;

#endif

