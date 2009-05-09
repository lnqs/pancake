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

#ifndef PC_STYLE_H
#define PC_STYLE_H

#include <gtk/gtkstyle.h>

G_BEGIN_DECLS

#define PC_TYPE_STYLE 			(pc_style_get_type())
#define PC_STYLE(object) \
		(G_TYPE_CHECK_INSTANCE_CAST((object), PC_TYPE_STYLE, PcStyle))
#define PC_STYLE_CLASS(klass) \
		(G_TYPE_CHECK_CLASS_CAST((klass), PC_TYPE_STYLE, PcStyleClass))
#define PC_IS_STYLE(object) \
		(G_TYPE_CHECK_INSTANCE_TYPE((object), PC_TYPE_STYLE))
#define PC_IS_STYLE_CLASS(klass) \
		(G_TYPE_CHECK_CLASS_TYPE((klass), PC_TYPE_STYLE))
#define PC_STYLE_GET_CLASS(obj) \
		(G_TYPE_INSTANCE_GET_CLASS((obj), PC_TYPE_STYLE, PcStyleClass))

/**
 * @todo docs
 **/
typedef struct PcStyle
{
	GtkStyle parent_instance;
} PcStyle;

/**
 * @todo docs
 **/
typedef struct PcStyleClass
{
	GtkStyleClass parent_class;
	void (*draw_panel)(GtkStyle* style, GdkWindow* window,
			GtkStateType state_type, GtkShadowType shadow_type,
			GdkRectangle* area, GtkWidget* widget, const gchar* detail,
			gint x, gint y, gint width, gint height);
} PcStyleClass;

/**
 * @todo docs
 **/
void pc_style_apply(GtkStyle* style, GtkWidget* widget);

/**
 * @todo docs
 **/
GType pc_style_get_type();

G_END_DECLS

#endif

