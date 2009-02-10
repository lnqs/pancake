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
#include "pc_types.h"
#include "pc_commandline.h"
#include "pc_configparser.h"
#include "pc_optionconv.h"

#ifdef GDK_WINDOWING_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#else
#error "Only GDKs X11-backend is supported currently"
#endif

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

G_DEFINE_TYPE(PcPanel, pc_panel, GTK_TYPE_WINDOW);

#define PC_PANEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), PC_TYPE_PANEL, PcPanelPrivate))

typedef struct PcPanelPrivate
{
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

static void pc_panel_class_init(PcPanelClass* class);
static void pc_panel_init(PcPanel* panel);
static gboolean pc_panel_configure_event(GtkWidget* widget,
		GdkEventConfigure* ev, gpointer data);
static void pc_panel_align_size_pos(PcPanel* panel);
static void pc_panel_update_strut(PcPanel* panel);

static void pc_panel_class_init(PcPanelClass* class)
{
	GObjectClass* obj_class = G_OBJECT_CLASS(class);
	g_type_class_add_private(obj_class, sizeof(PcPanelPrivate));
}

static void pc_panel_init(PcPanel* panel)
{
	PcPanelPrivate* priv = PC_PANEL_GET_PRIVATE(panel);

	priv->width = 0.95f;
	priv->height = 24;
	priv->align = PC_ALIGN_BOTTOM;
	priv->strut_enabled = TRUE;

	gtk_window_stick(GTK_WINDOW(panel));
	gtk_window_set_type_hint(GTK_WINDOW(panel), GDK_WINDOW_TYPE_HINT_DOCK);
	gtk_widget_set_app_paintable(GTK_WIDGET(panel), TRUE);
	gtk_window_set_decorated(GTK_WINDOW(panel), FALSE);
	gtk_window_set_deletable(GTK_WINDOW(panel), FALSE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(panel), TRUE);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(panel), TRUE);
	wnck_set_client_type(WNCK_CLIENT_TYPE_PAGER);

	g_signal_connect(G_OBJECT(panel), "configure-event",
			G_CALLBACK(pc_panel_configure_event), NULL);
}

GtkWidget* pc_panel_new()
{
	return GTK_WIDGET(g_object_new(PC_TYPE_PANEL, NULL));
}

static gboolean pc_panel_configure_event(GtkWidget* widget,
		GdkEventConfigure* ev, gpointer data)
{
	pc_panel_align_size_pos(PC_PANEL(widget));
	pc_panel_update_strut(PC_PANEL(widget));

	return TRUE;
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

		// TODO: Add partitial strut
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

gboolean pc_panel_set_option(
		PcPanel* panel, const gchar* key, const gchar* value)
{
	if(!g_strcmp0(key, "alignment"))
	{
		PcAlignment align;
		if(!pc_optionconv_alignment(value, &align))
			return FALSE;

		pc_panel_set_align(panel, align);
	}
	else if(!g_strcmp0(key, "width"))
	{
		gfloat width;
		if(!pc_optionconv_float(value, &width))
			return FALSE;

		pc_panel_set_width(panel, width);
	}
	else if(!g_strcmp0(key, "height"))
	{
		gint height;
		if(!pc_optionconv_int(value, &height))
			return FALSE;

		pc_panel_set_height(panel, height);
	}
	else if(!g_strcmp0(key, "strut_enabled"))
	{
		gboolean enabled;
		if(!pc_optionconv_bool(value, &enabled))
			return TRUE;

		pc_panel_set_strut_enabled(panel, enabled);
	}
	else
	{
		g_print("%s: invalid panel-option %s\n",
				pc_program_invocation_name, key);
		return FALSE;
	}

	return TRUE;
}

