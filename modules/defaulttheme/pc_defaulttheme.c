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
#include <pc_style.h>

#define PC_TYPE_DEFAULTTHEME_STYLE (pc_defaulttheme_style_get_type())
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
	gfloat bg_alpha;
} PcDefaultthemeStyle;

typedef struct PcDefaultthemeStyleClass
{
	PcStyleClass parent_class;
} PcDefaultthemeStyleClass;

G_DEFINE_TYPE(PcDefaultthemeStyle, pc_defaulttheme_style, PC_TYPE_STYLE);

/* This function is taken from gtks gtkstyle.c */
static void pc_defaulttheme_sanitize_size(GdkWindow* window,
		gint* width, gint* height)
{
	if((*width == -1) && (*height == -1))
		gdk_drawable_get_size(window, width, height);
	else if(*width == -1)
		gdk_drawable_get_size(window, width, NULL);
	else if(*height == -1)
		gdk_drawable_get_size(window, NULL, height);
}

static void pc_defaulttheme_draw_flat_box(GtkStyle* style, GdkWindow* window,
		 GtkStateType state_type, GtkShadowType shadow_type,
		 GdkRectangle* area, GtkWidget* widget, const gchar* detail,
		 gint x, gint y, gint width, gint height)
{
	PcDefaultthemeStyle* self = PC_DEFAULTTHEME_STYLE(style);

	pc_defaulttheme_sanitize_size(window, &width, &height);

	gfloat alpha = self->bg_alpha;
	/* I have absolutly no idea why it doesn't work for tooltips to have
	   transparency here, but for now this should do it. */
	/* TODO: find out why it doesn't work and fix it, or, at least, find out
	         for what to check to set full alpha by this instead of having to
	         check for the detail o.O */
	if(!g_strcmp0(detail, "tooltip"))
		alpha = 1.0f;

	cairo_t* cr = gdk_cairo_create(window);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

	cairo_set_source_rgba(cr,
			(gfloat)style->bg[state_type].red   / 65535.0f,
			(gfloat)style->bg[state_type].green / 65535.0f,
			(gfloat)style->bg[state_type].blue  / 65535.0f,
			alpha);
	cairo_rectangle(cr, x, y, width, height);
	cairo_fill(cr);

	cairo_destroy(cr);
}

static void pc_defaulttheme_draw_box(GtkStyle* style, GdkWindow* window,
		GtkStateType state_type, GtkShadowType shadow_type, GdkRectangle* area,
		GtkWidget* widget, const gchar* detail, gint x, gint y,
		gint width, gint height)
{
	PcDefaultthemeStyle* self = PC_DEFAULTTHEME_STYLE(style);

	pc_defaulttheme_sanitize_size(window, &width, &height);

	gfloat alpha = self->bg_alpha;
	/* I have absolutly no idea why it doesn't work for tooltips to have
	   transparency here, but for now this should do it. */
	/* TODO: find out why it doesn't work and fix it, or, at least, find out
	         for what to check to set full alpha by this instead of having to
	         check for the detail o.O */
	if(!g_strcmp0(detail, "menu") || !g_strcmp0(detail, "menuitem"))
		alpha = 1.0f;

	cairo_t* cr = gdk_cairo_create(window);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

	cairo_set_source_rgba(cr,
			(1.0f - (gfloat)style->bg[state_type].red)   / 65535.0f,
			(1.0f - (gfloat)style->bg[state_type].green) / 65535.0f,
			(1.0f - (gfloat)style->bg[state_type].blue) / 65535.0f,
			1.0f);

	cairo_rectangle(cr, x + 1, y + 1, width - 2, height - 2);
	cairo_stroke_preserve(cr);

	cairo_set_source_rgba(cr,
			(gfloat)style->bg[state_type].red   / 65535.0f,
			(gfloat)style->bg[state_type].green / 65535.0f,
			(gfloat)style->bg[state_type].blue  / 65535.0f,
			alpha);

	cairo_fill(cr);

	cairo_destroy(cr);
}

static void pc_defaulttheme_draw_hline(GtkStyle* style, GdkWindow* window,
		GtkStateType state_type, GdkRectangle* area, GtkWidget* widget,
		const gchar* detail, gint x1, gint x2, gint y)
{
	cairo_t* cr = gdk_cairo_create(window);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

	cairo_set_line_width(cr, 0.5f);
	cairo_set_source_rgba(cr,
			(1.0f - (gfloat)style->bg[state_type].red)   / 65535.0f,
			(1.0f - (gfloat)style->bg[state_type].green) / 65535.0f,
			(1.0f - (gfloat)style->bg[state_type].blue) / 65535.0f,
			1.0f);

	cairo_move_to(cr, x1, y);
	cairo_line_to(cr, x2, y);
	cairo_stroke(cr);

	cairo_destroy(cr);
}

static void pc_defaulttheme_draw_vline(GtkStyle* style, GdkWindow* window,
		GtkStateType state_type, GdkRectangle* area, GtkWidget* widget,
		const gchar* detail, gint y1, gint y2, gint x)
{
	cairo_t* cr = gdk_cairo_create(window);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

	cairo_set_line_width(cr, 0.5f);
	cairo_set_source_rgba(cr,
			(1.0f - (gfloat)style->bg[state_type].red)   / 65535.0f,
			(1.0f - (gfloat)style->bg[state_type].green) / 65535.0f,
			(1.0f - (gfloat)style->bg[state_type].blue) / 65535.0f,
			1.0f);

	cairo_move_to(cr, x, y1);
	cairo_line_to(cr, x, y2);
	cairo_stroke(cr);

	cairo_destroy(cr);
}

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
	cairo_set_source_rgba(cr,
			(gfloat)style->bg[state_type].red   / 65535.0f,
			(gfloat)style->bg[state_type].green / 65535.0f,
			(gfloat)style->bg[state_type].blue  / 65535.0f,
			self->bg_alpha);

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
	GtkStyleClass* gtk_style_class = GTK_STYLE_CLASS(class);

	gtk_style_class->draw_box = &pc_defaulttheme_draw_box;
	gtk_style_class->draw_flat_box = &pc_defaulttheme_draw_flat_box;
	gtk_style_class->draw_hline = &pc_defaulttheme_draw_hline;
	gtk_style_class->draw_vline = &pc_defaulttheme_draw_vline;

	pc_style_class->draw_panel = &pc_defaulttheme_draw_panel;
}

static void pc_defaulttheme_style_init(PcDefaultthemeStyle* style)
{
}

static GtkStyle* pc_defaulttheme_style_new()
{
	return GTK_STYLE(g_object_new(PC_TYPE_DEFAULTTHEME_STYLE, NULL));
}

static PcColor pc_defaulttheme_config_to_color(
		Config* options, const gchar* key)
{
	PcColor color = { 0, };
	
	if(cfg_size(options, key) == 3)
	{
		color.r = cfg_getnfloat(options, key, 0);
		color.g = cfg_getnfloat(options, key, 1);
		color.b = cfg_getnfloat(options, key, 2);
	}
	else
		g_critical("Invalid format of '%s'", key);

	return color;
}

static GtkStyle* pc_defaulttheme_instantiate(Config* options)
{
	PcStyle* style = PC_STYLE(pc_defaulttheme_style_new());

	const gchar* font = cfg_getstr(options, "font");
	if(font)
		GTK_STYLE(style)->font_desc = pango_font_description_from_string(font);

	PcColor fg_normal = pc_defaulttheme_config_to_color(options, "fg_normal");
	GTK_STYLE(style)->fg[GTK_STATE_NORMAL].red = fg_normal.r       * 65535;
	GTK_STYLE(style)->fg[GTK_STATE_NORMAL].green = fg_normal.g     * 65535;
	GTK_STYLE(style)->fg[GTK_STATE_NORMAL].blue = fg_normal.b      * 65535;

	PcColor bg_normal = pc_defaulttheme_config_to_color(options, "bg_normal");
	GTK_STYLE(style)->bg[GTK_STATE_NORMAL].red = bg_normal.r       * 65535;
	GTK_STYLE(style)->bg[GTK_STATE_NORMAL].green = bg_normal.g     * 65535;
	GTK_STYLE(style)->bg[GTK_STATE_NORMAL].blue = bg_normal.b      * 65535;

	PcColor fg_active = pc_defaulttheme_config_to_color(options, "fg_active");
	GTK_STYLE(style)->fg[GTK_STATE_ACTIVE].red = fg_active.r       * 65535;
	GTK_STYLE(style)->fg[GTK_STATE_ACTIVE].green = fg_active.g     * 65535;
	GTK_STYLE(style)->fg[GTK_STATE_ACTIVE].blue = fg_active.b      * 65535;

	PcColor bg_active = pc_defaulttheme_config_to_color(options, "bg_active");
	GTK_STYLE(style)->bg[GTK_STATE_ACTIVE].red = bg_active.r       * 65535;
	GTK_STYLE(style)->bg[GTK_STATE_ACTIVE].green = bg_active.g     * 65535;
	GTK_STYLE(style)->bg[GTK_STATE_ACTIVE].blue = bg_active.b      * 65535;

	PcColor fg_prelight = pc_defaulttheme_config_to_color(
			options, "fg_prelight");
	GTK_STYLE(style)->fg[GTK_STATE_PRELIGHT].red = fg_prelight.r   * 65535;
	GTK_STYLE(style)->fg[GTK_STATE_PRELIGHT].green = fg_prelight.g * 65535;
	GTK_STYLE(style)->fg[GTK_STATE_PRELIGHT].blue = fg_prelight.b  * 65535;

	PcColor bg_prelight = pc_defaulttheme_config_to_color(
			options, "bg_prelight");
	GTK_STYLE(style)->bg[GTK_STATE_PRELIGHT].red = bg_prelight.r   * 65535;
	GTK_STYLE(style)->bg[GTK_STATE_PRELIGHT].green = bg_prelight.g * 65535;
	GTK_STYLE(style)->bg[GTK_STATE_PRELIGHT].blue = bg_prelight.b  * 65535;

	PC_DEFAULTTHEME_STYLE(style)->rounded_top = cfg_getbool(
			options, "rounded_top");
	PC_DEFAULTTHEME_STYLE(style)->rounded_bottom = cfg_getbool(
			options, "rounded_bottom");
	PC_DEFAULTTHEME_STYLE(style)->bg_alpha = cfg_getfloat(
			options, "bg_alpha");

	return GTK_STYLE(style);
}

static ConfigOption pc_defaulttheme_options[] = {
	CFG_BOOL("rounded_top", TRUE, CFGF_NONE),
	CFG_BOOL("rounded_bottom", FALSE, CFGF_NONE),
	CFG_STR("font", NULL, CFGF_NONE),
	CFG_FLOAT_LIST("fg_normal", "{0.6, 0.6, 0.6}", CFGF_NONE),
	CFG_FLOAT_LIST("bg_normal", "{0.2, 0.2, 0.2}", CFGF_NONE),
	CFG_FLOAT_LIST("fg_active", "{0.9, 0.9, 0.9}", CFGF_NONE),
	CFG_FLOAT_LIST("bg_active", "{0.2, 0.2, 0.2}", CFGF_NONE),
	CFG_FLOAT_LIST("fg_prelight", "{1.0, 1.0, 1.0}", CFGF_NONE),
	CFG_FLOAT_LIST("bg_prelight", "{0.2, 0.2, 0.2}", CFGF_NONE),
	CFG_FLOAT("bg_alpha", 0.75f, CFGF_NONE),
	CFG_END()
};

void pc_defaulttheme_print_help()
{
	printf(
"# This is the standardtheme of pancake\n"
"defaulttheme mytheme\n"
"{\n"
"    # set to 'true' to have the upper edges of the panel rounded\n"
"    rounded_top = true\n"
"\n"
"    # set to 'true' to have the bottom edges of the panel rounded\n"
"    rounded_bottom = false\n"
"\n"
"    # The font to use\n"
"    font = \"Sans 10\"\n"
"\n"
"    # The foreground-color to use, as red-, green- and blue-values in range\n"
"    # 0.0 to 1.0.\n"
"    fg_normal = { 0.6, 0.6, 0.6 }\n"
"\n"
"    # The background-color to use\n"
"    bg_normal = { 0.2, 0.2, 0.2 }\n"
"\n"
"    # The foreground-color to use for active widgets\n"
"    fg_active = { 0.9, 0.9, 0.9 }\n"
"\n"
"    # The background-color to use for active widgets\n"
"    bg_active = { 0.2, 0.2, 0.2 }\n"
"\n"
"    # The foreground-color to use for prelighted widgets\n"
"    fg_prelight = { 1.0, 1.0, 1.0 }\n"
"\n"
"    # The background-color to use for prelighted widgets\n"
"    bg_prelight = { 0.2, 0.2, 0.2 }\n"
"\n"
"    # alpha-value in range 0.0 to 1.0 for the panel's background\n"
"    bg_alpha = 0.75\n"
"}\n"
"\n"
	);
}

static const PcThemeInfo pc_defaulttheme_info = {
	.name = "defaulttheme",
	.instantiate = &pc_defaulttheme_instantiate,
	.print_help = &pc_defaulttheme_print_help,
	.options = pc_defaulttheme_options
};

gboolean pc_module_init(const PcModuleCallbacks* callbacks)
{
	callbacks->register_theme(&pc_defaulttheme_info);
	return TRUE;
}

