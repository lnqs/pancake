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
#include <pc_module.h>

#define PC_TYPE_DEFAULTTHEME_STYLE  (pc_defaulttheme_style_get_type())
#define PC_DEFAULTTHEME_STYLE(object) \
		(G_TYPE_CHECK_INSTANCE_CAST((object), \
		PC_TYPE_DEFAULTTHEME_STYLE, PcDefaultthemeStyle))
#define PC_DEFAULTTHEME_STYLE_CLASS(klass) \
		(G_TYPE_CHECK_CLASS_CAST((klass), \
		PC_TYPE_DEFAULTTHEME_STYLE, PcDefaultthemeStyleClass))
#define PC_DEFAULTTHEME_IS_STYLE(object) \
		(G_TYPE_CHECK_INSTANCE_TYPE((object), PC_TYPE_DEFAULTTHEME_STYLE))
#define PC_IS_DEFAULTTHEME_STYLE_CLASS(klass) \
		(G_TYPE_CHECK_CLASS_TYPE((klass), PC_TYPE_DEFAULTTHEME_STYLE))
#define PC_DEFAULTTHEME_STYLE_GET_CLASS(obj) \
		(G_TYPE_INSTANCE_GET_CLASS((obj), \
		PC_TYPE_DEFAULTTHEME_STYLE, PcDefaultthemeStyleClass))

typedef struct PcDefaultthemeStyle
{
	PcStyle parent_instance;
	gboolean rounded_top;
	gboolean rounded_bottom;
} PcDefaultthemeStyle;

typedef struct PcDefaultthemeStyleClass
{
	PcStyleClass parent_class;
	void (*draw_panel)(GtkStyle* style, GdkWindow* window,
			GtkStateType state_type, GtkShadowType shadow_type,
			GdkRectangle* area, GtkWidget* widget, const gchar* detail,
			gint x, gint y, gint width, gint height);
} PcDefaultthemeStyleClass;

G_DEFINE_TYPE(PcDefaultthemeStyle, pc_defaulttheme_style, PC_TYPE_STYLE);

static void pc_defaulttheme_draw_panel(GtkStyle* style, GdkWindow* window,
			GtkStateType state_type, GtkShadowType shadow_type,
			GdkRectangle* area, GtkWidget* widget, const gchar* detail,
			gint x, gint y, gint width, gint height)
{
	PcDefaultthemeStyle* self = PC_DEFAULTTHEME_STYLE(style);
	
	gint w = widget->allocation.width;
	gint h = widget->allocation.height;

	cairo_t* cr = gdk_cairo_create(window);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

	cairo_set_source_rgba(cr, 0.0f, 0.0f, 0.0f, 0.0f);
	cairo_rectangle(cr, 0, 0, w, h);
	cairo_fill(cr);

	gint corner_size = h * 0.75f;
	cairo_set_source_rgb(cr,
			(float)style->bg[state_type].red   / 65535.0f,
			(float)style->bg[state_type].green / 65535.0f,
			(float)style->bg[state_type].blue  / 65535.0f);

	cairo_move_to(cr, 0, corner_size);
	if(self->rounded_top)
		cairo_curve_to(cr, 0, 0, 0, 0, corner_size, 0);
	else
		cairo_line_to(cr, 0, 0);
	
	cairo_line_to(cr, w - corner_size, 0);
	if(self->rounded_top)
		cairo_curve_to(cr, w, 0, w, 0, w, corner_size);
	else
		cairo_line_to(cr, w, 0);
	
	cairo_line_to(cr, w, h - corner_size);
	if(self->rounded_bottom)
		cairo_curve_to(cr, w, h, w, h, w - corner_size, h);
	else
		cairo_line_to(cr, w, h);
	
	cairo_line_to(cr, corner_size, h);
	if(self->rounded_bottom)
		cairo_curve_to(cr, 0, h, 0, h, 0, h - corner_size);
	else
		cairo_line_to(cr, 0, h);
	
	cairo_close_path(cr);
	cairo_fill(cr);

	cairo_destroy(cr);
}

static void pc_defaulttheme_style_class_init(PcDefaultthemeStyleClass* class)
{
	PcStyleClass* pc_style_class = PC_STYLE_CLASS(class);
	pc_style_class->draw_panel = &pc_defaulttheme_draw_panel;
}

static void pc_defaulttheme_style_init(PcDefaultthemeStyle* style)
{
	style->rounded_top = TRUE;
	style->rounded_bottom = FALSE;
}

static GtkStyle* pc_defaulttheme_style_new()
{
	return GTK_STYLE(g_object_new(PC_TYPE_DEFAULTTHEME_STYLE, NULL));
}

static PancakeTheme pc_defaulttheme = {
	.name       = "defaulttheme",
	.init       = NULL,
	.fini       = NULL,
	.new_style  = &pc_defaulttheme_style_new
};

PANCAKE_THEME(pc_defaulttheme)
