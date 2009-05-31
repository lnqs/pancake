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

#define PC_STYLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PC_TYPE_STYLE, PcStylePrivate))

enum
{
	PROP_0,
	PROP_WIDGET_PADDING,
	PROP_BORDER_PADDING,
	PROP_LAST
};

static void pc_style_set_property(GObject* object,
		guint prop_id, const GValue* value, GParamSpec* pspec)
{
	switch(prop_id)
	{
		case PROP_WIDGET_PADDING:
			if(g_value_get_int(value))
	    		PC_STYLE(object)->widget_padding = g_value_get_int(value);
			break;

		case PROP_BORDER_PADDING:
			if(g_value_get_int(value))
	    		PC_STYLE(object)->border_padding = g_value_get_int(value);
			break;
	
		default:
	    	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
    }
}

static void pc_style_get_property(GObject* object,
		guint prop_id, GValue* value, GParamSpec* pspec)
{
	switch(prop_id)
	{
		case PROP_WIDGET_PADDING:
			g_value_set_int(value, PC_STYLE(object)->widget_padding);
			break;

		case PROP_BORDER_PADDING:
			g_value_set_int(value, PC_STYLE(object)->border_padding);
			break;
	
		default:
	    	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
    }
}

static void pc_style_class_init(PcStyleClass* class)
{
	GObjectClass* obj_class = G_OBJECT_CLASS(class);

	class->draw_panel = NULL;

	obj_class->set_property = pc_style_set_property;
	obj_class->get_property = pc_style_get_property;

	g_object_class_install_property(obj_class, PROP_WIDGET_PADDING,
			g_param_spec_int("widget-padding",
					"Padding between widgets",
					"The padding between widgets in the panel. Default: 4",
					0, G_MAXINT, 4,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property(obj_class, PROP_BORDER_PADDING,
			g_param_spec_int("border-padding",
					"Padding between panel's borders and widgets",
					"The padding between the panels's left and right borders "
					"and the widgets within. Default: 4",
					0, G_MAXINT, 4,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void pc_style_init(PcStyle* style)
{
	/*TODO: This sucks. Replace it with something that'll work */
	GTK_STYLE(style)->ythickness = -5;
}

static void pc_style_apply_callback(GtkWidget* widget, gpointer data)
{
	GtkStyle* style = GTK_STYLE(data);
	gtk_widget_set_style(widget, style);

	if(GTK_IS_CONTAINER(widget))
	{
		gtk_container_forall(GTK_CONTAINER(widget),
				&pc_style_apply_callback, style);
	}
}

void pc_style_apply(GtkStyle* style, GtkWidget* widget)
{
	pc_style_apply_callback(widget, style);
}

guint pc_style_get_widget_padding(GtkStyle* style)
{
	if(PC_IS_STYLE(style))
	{
		GValue value = {0, };
		g_value_init(&value, G_TYPE_INT);
		g_object_get_property(G_OBJECT(style), "widget-padding", &value);
		guint padding = g_value_get_int(&value);
		g_value_unset(&value);
		return padding;
	}
	else
		return 0;
}

guint pc_style_get_border_padding(GtkStyle* style)
{
	if(PC_IS_STYLE(style))
	{
		GValue value = {0, };
		g_value_init(&value, G_TYPE_INT);
		g_object_get_property(G_OBJECT(style), "border-padding", &value);
		guint padding = g_value_get_int(&value);
		g_value_unset(&value);
		return padding;
	}
	else
		return 0;
}

