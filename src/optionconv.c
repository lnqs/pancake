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

#include "optionconv.h"
#include <stdlib.h>
#include "misc.h"

gboolean pc_optionconv_alignment(const gchar* value, PcAlignment* target)
{
	if(!g_strcmp0(value, "top"))
		*target = PC_ALIGN_TOP;
	else if(!g_strcmp0(value, "bottom"))
		*target = PC_ALIGN_BOTTOM;
	else
	{
		g_print("%s: failed to parse alignment from %s\n",
				pc_program_invocation_name, value);
		return FALSE;
	}

	return TRUE;
}

gboolean pc_optionconv_float(const gchar* value, gfloat* target)
{
	gchar* endptr;
	float res = strtod(value, &endptr);
	if(*endptr)
	{
		g_print("%s: failed to parse float from %s\n",
				pc_program_invocation_name, value);
		return FALSE;
	}

	*target = res;
	return TRUE;
}

gboolean pc_optionconv_int(const gchar* value, gint* target)
{
	gchar* endptr;
	long int res = strtol(value, &endptr, 0);
	if(*endptr)
	{
		g_print("%s: failed to parse int from %s\n",
				pc_program_invocation_name, value);
		return FALSE;
	}

	*target = res;
	return TRUE;
}

gboolean pc_optionconv_bool(const gchar* value, gboolean* target)
{
	if(!g_strcmp0(value, "true"))
		*target = TRUE;
	else if(!g_strcmp0(value, "false"))
		*target = FALSE;
	else
	{
		g_print("%s: failed to parse boolean from %s\n",
				pc_program_invocation_name, value);
		return FALSE;
	}

	return TRUE;
}


