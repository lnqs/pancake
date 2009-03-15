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

#include "pc_style.h"
#include <gtk/gtk.h>

G_DEFINE_TYPE(PcStyle, pc_style, GTK_TYPE_STYLE);

#define PC_STYLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), PC_TYPE_STYLE, PcStylePrivate))

static void pc_style_class_init(PcStyleClass* klass)
{
	klass->draw_panel = NULL;
}

static void pc_style_init(PcStyle* style)
{
}

static void pc_style_apply_callback(GtkWidget* widget, gpointer data)
{
	GtkStyle* style = GTK_STYLE(data);
	gtk_widget_set_style(widget, style);

	if(GTK_IS_CONTAINER(widget))
		gtk_container_forall(GTK_CONTAINER(widget),
				&pc_style_apply_callback, style);
}

void pc_style_apply(GtkStyle* style, GtkWidget* widget)
{
	pc_style_apply_callback(widget, style);
}
