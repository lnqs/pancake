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
		PC_DEFAULTTHEME_TYPE_STYLE, PcDefaultthemeStyle))
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

static void pc_defaulttheme_style_class_init(PcDefaultthemeStyleClass* klass)
{
	klass->draw_panel = NULL;
}

static void pc_defaulttheme_style_init(PcDefaultthemeStyle* style)
{
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

