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

#include "pc_panel.h"
#include <pc_style.h>
#include <pc_types.h>
#include "pc_commandline.h"
#include "pc_configparser.h"
#include "pc_modloader.h"

#ifdef GDK_WINDOWING_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#else
#error "Only the X11-backend of GDK is supported currently"
#endif

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

G_DEFINE_TYPE(PcPanel, pc_panel, GTK_TYPE_WINDOW);

enum
{
	PROP_0,
	PROP_ALIGN,
	PROP_WIDTH,
	PROP_HEIGHT,
	PROP_STRUT_ENABLED,
	PROP_LAST
};

#define PC_PANEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), PC_TYPE_PANEL, PcPanelPrivate))

typedef struct PcPanelPrivate
{
	GtkWidget* box;
	GtkWidget* alignment;
	PcAlignment align;
	gfloat width;
	gint height;
	gboolean strut_enabled;
} PcPanelPrivate;

enum
{
	STRUT_LEFT = 0,
	STRUT_RIGHT = 1,
	STRUT_TOP = 2,
	STRUT_BOTTOM = 3
};

static void pc_panel_align_size_pos(PcPanel* panel)
{
	PcPanelPrivate* priv = PC_PANEL_GET_PRIVATE(panel);

	gint scr_w = gdk_screen_get_width(
			gtk_window_get_screen(GTK_WINDOW(panel)));
	gint scr_h = gdk_screen_get_height(
			gtk_window_get_screen(GTK_WINDOW(panel)));

	gint w = scr_w * priv->width;
	gint h = priv->height;

	gint x = (scr_w - w) / 2;
	gint y = (priv->align == PC_ALIGN_TOP ? 0 : scr_h - h);

	int curr_w, curr_h;
	gtk_window_get_size(GTK_WINDOW(panel), &curr_w, &curr_h);

	if(curr_w != w || curr_h != h)
	{
		GdkGeometry geom = {
			.min_width = w,
			.min_height = h,
			.max_width = w,
			.max_height = h,
		};
		gtk_window_set_geometry_hints(GTK_WINDOW(panel),
				GTK_WIDGET(panel), &geom,
				GDK_HINT_POS | GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE);
	}

	int curr_x, curr_y;
	gtk_window_get_position(GTK_WINDOW(panel), &curr_x, &curr_y);
	if(curr_x != x || curr_y != y)
		gtk_window_move(GTK_WINDOW(panel), x, y);
}

static void pc_panel_update_strut(PcPanel* panel)
{
	if(!GTK_WIDGET_REALIZED(panel))
		gtk_widget_realize(GTK_WIDGET(panel));
	
	PcPanelPrivate* priv = PC_PANEL_GET_PRIVATE(panel);

	Display* display = GDK_DISPLAY_XDISPLAY(
			gtk_widget_get_display(GTK_WIDGET(panel)));
	Window window = GDK_WINDOW_XWINDOW(GTK_WIDGET(panel)->window);
	Atom net_wm_strut = XInternAtom(display, "_NET_WM_STRUT", False);

	if(priv->strut_enabled)
	{
		int w, h;
		gtk_window_get_size(GTK_WINDOW(panel), &w, &h);
	
		gulong strut[12] = { 0, };

		/* TODO: Add partitial strut */
		if(priv->align == PC_ALIGN_BOTTOM)
			strut[STRUT_BOTTOM] = h;
		else
			strut[STRUT_TOP] = h;

		gdk_error_trap_push();
		XChangeProperty(display, window, net_wm_strut, XA_CARDINAL,
				32, PropModeReplace, (guchar*)&strut, 4);
		gdk_error_trap_pop();
	}
	else
	{
		gdk_error_trap_push();
		XDeleteProperty(display, window, net_wm_strut);
		gdk_error_trap_pop();
	}
}

static gboolean pc_panel_configure_event(GtkWidget* widget,
		GdkEventConfigure* ev, gpointer data)
{
	pc_panel_align_size_pos(PC_PANEL(widget));
	pc_panel_update_strut(PC_PANEL(widget));

	return TRUE;
}

static void pc_panel_expose_child(GtkWidget* child, gpointer client_data)
{
	struct {
		GtkWidget* container;
		GdkEventExpose* event;
	}* data = client_data;

	gtk_container_propagate_expose(GTK_CONTAINER(data->container),
			child, data->event);
}

static gboolean pc_panel_expose(GtkWidget* widget, GdkEventExpose* ev)
{
	if(PC_IS_STYLE(widget->style) && PC_STYLE_GET_CLASS(
				widget->style)->draw_panel)
	{
		gint border_width = GTK_CONTAINER(widget)->border_width;

		(*PC_STYLE_GET_CLASS(widget->style)->draw_panel)(
				widget->style,
				widget->window,
				GTK_STATE_NORMAL,
				GTK_SHADOW_OUT,
				&ev->area, widget,
				NULL,
				widget->allocation.x,
				widget->allocation.y,
				widget->allocation.width - border_width * 2,
				widget->allocation.height - border_width * 2);
	}
	else
	{
		gtk_paint_flat_box(widget->style, widget->window, GTK_STATE_NORMAL,
				GTK_SHADOW_NONE, &ev->area, widget, "base", 0, 0, -1, -1);
	}

	struct {
    	GtkWidget *container;
	    GdkEventExpose *event;
	} data;

	data.container = widget;
	data.event = ev;

	gtk_container_forall(GTK_CONTAINER(widget),
			pc_panel_expose_child, &data);
	
	return TRUE;
}

static void pc_panel_set_property(GObject* object,
		guint prop_id, const GValue* value, GParamSpec* pspec)
{
	PcPanel* panel = PC_PANEL(object);
	PcPanelPrivate* priv = PC_PANEL_GET_PRIVATE(object);
	
	switch(prop_id)
	{
		case PROP_ALIGN:
			priv->align = g_value_get_int(value);
			pc_panel_align_size_pos(panel);
			break;

		case PROP_WIDTH:
			priv->width = g_value_get_float(value);
			pc_panel_align_size_pos(panel);
			pc_panel_update_strut(panel);
			break;

		case PROP_HEIGHT:
			priv->height = g_value_get_int(value);
			pc_panel_align_size_pos(panel);
			pc_panel_update_strut(panel);
			break;

		case PROP_STRUT_ENABLED:
			priv->strut_enabled = g_value_get_boolean(value);
			pc_panel_update_strut(panel);
			break;

		default:
	    	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
    }
}

static void pc_panel_get_property(GObject* object,
		guint prop_id, GValue* value, GParamSpec* pspec)
{
	PcPanelPrivate* priv = PC_PANEL_GET_PRIVATE(object);

	switch(prop_id)
	{
		case PROP_ALIGN:
			g_value_set_int(value, priv->align);
			break;

		case PROP_WIDTH:
			g_value_set_float(value, priv->width);
			break;

		case PROP_HEIGHT:
			g_value_set_int(value, priv->height);
			break;

		case PROP_STRUT_ENABLED:
			g_value_set_boolean(value, priv->strut_enabled);
			break;
	
		default:
	    	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
    }
}

static void pc_panel_class_init(PcPanelClass* class)
{
	GObjectClass* obj_class = G_OBJECT_CLASS(class);
	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

	obj_class->set_property = &pc_panel_set_property;
	obj_class->get_property = &pc_panel_get_property;

	widget_class->expose_event = pc_panel_expose;

	g_object_class_install_property(obj_class, PROP_ALIGN,
			g_param_spec_int("align",
					"Alignment of the panel",
					"The panel's alignment. Default: BOTTOM",
					PC_ALIGN_0, PC_ALIGN_LAST,
					PC_ALIGN_BOTTOM,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(obj_class, PROP_WIDTH,
			g_param_spec_float("width",
					"The panel's width",
					"The panel's width, relativ to the width of screen in "
							"range 0.0f..1.0f. Default: 0.98f",
					0.0f, 1.0f,
					0.98f,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(obj_class, PROP_HEIGHT,
			g_param_spec_int("height",
					"The panel's height",
					"The panel's height in pixels. Default: 24",
					0, G_MAXINT,
					24,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(obj_class, PROP_STRUT_ENABLED,
			g_param_spec_boolean("strut-enabled",
					"if the panel shall set a strut",
					"Holds if the panel shall set a strut. Default: TRUE",
					TRUE,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_type_class_add_private(obj_class, sizeof(PcPanelPrivate));
}

static void pc_panel_init(PcPanel* panel)
{
	PcPanelPrivate* private = PC_PANEL_GET_PRIVATE(panel);
	
	gtk_window_stick(GTK_WINDOW(panel));
	gtk_window_set_type_hint(GTK_WINDOW(panel), GDK_WINDOW_TYPE_HINT_DOCK);
	gtk_widget_set_app_paintable(GTK_WIDGET(panel), TRUE);
	gtk_window_set_decorated(GTK_WINDOW(panel), FALSE);
	gtk_window_set_deletable(GTK_WINDOW(panel), FALSE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(panel), TRUE);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(panel), TRUE);
	wnck_set_client_type(WNCK_CLIENT_TYPE_PAGER);

	GdkScreen* screen = gtk_widget_get_screen(GTK_WIDGET(panel));
	GdkColormap* colormap = gdk_screen_get_rgba_colormap(screen);

	if(!colormap)
		colormap = gdk_screen_get_rgb_colormap(screen);
	
	gtk_widget_set_colormap(GTK_WIDGET(panel), colormap);

	g_signal_connect(G_OBJECT(panel), "configure-event",
			G_CALLBACK(pc_panel_configure_event), NULL);

	private->alignment = gtk_alignment_new(0.5f, 0.5f, 1.0f, 1.0f);
	private->box = gtk_hbox_new(FALSE, 3);
	gtk_container_add(GTK_CONTAINER(panel), private->alignment);
	gtk_container_add(GTK_CONTAINER(private->alignment), private->box);
}

GtkWidget* pc_panel_new()
{
	return GTK_WIDGET(g_object_new(PC_TYPE_PANEL, NULL));
}

void pc_panel_set_width(PcPanel* panel, gfloat width)
{
	PC_PANEL_GET_PRIVATE(panel)->width = width;
	pc_panel_align_size_pos(panel);
	pc_panel_update_strut(panel);
}

void pc_panel_set_height(PcPanel* panel, gint height)
{
	PC_PANEL_GET_PRIVATE(panel)->height = height;
	pc_panel_align_size_pos(panel);
	pc_panel_update_strut(panel);
}

void pc_panel_set_align(PcPanel* panel, PcAlignment align)
{
	PC_PANEL_GET_PRIVATE(panel)->align = align;
	pc_panel_align_size_pos(panel);
	pc_panel_update_strut(panel);
}

void pc_panel_set_strut_enabled(PcPanel* panel, gboolean enabled)
{
	PC_PANEL_GET_PRIVATE(panel)->strut_enabled = enabled;
	pc_panel_update_strut(panel);
}

GtkBox* pc_panel_get_box(PcPanel* panel)
{
	return GTK_BOX(PC_PANEL_GET_PRIVATE(panel)->box);
}

void pc_panel_set_border_padding(PcPanel* panel, guint padding)
{
	GtkWidget* alignment = PC_PANEL_GET_PRIVATE(panel)->alignment;
	gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 0, padding, padding);
}

