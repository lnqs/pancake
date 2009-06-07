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

/* Most of the code in this file is taken from Anders Carlsson's
 * eggtraymanager, which is originally licensed unter the LGPL */

#include <pc_module.h>
#include <pc_misc.h>

#ifdef GDK_WINDOWING_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#else
#error "Only GDKs X11-backend is supported currently"
#endif

#define PC_TYPE_TRAYAREA	(pc_trayarea_get_type())
#define PC_TRAYAREA(obj) \
		(G_TYPE_CHECK_INSTANCE_CAST((obj), PC_TYPE_TRAYAREA, PcTrayarea))
#define PC_TRAYAREA_CLASS(obj) \
		(G_TYPE_CHECK_CLASS_CAST((obj), PC_TRAYAREA, PcTrayareaClass))
#define PC_IS_TRAYAREA(obj) \
		(G_TYPE_CHECK_INSTANCE_TYPE((obj), PC_TYPE_TRAYAREA))
#define PC_IS_TRAYAREA_CLASS(obj) \
		(G_TYPE_CHECK_CLASS_TYPE((obj), PC_TYPE_TRAYAREA))
#define PC_TRAYAREA_GET_CLASS \
		(G_TYPE_INSTANCE_GET_CLASS((obj), PC_TYPE_TRAYAREA, PcTrayareaClass))

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

typedef struct PcTrayarea
{
	GtkHBox parent;
} PcTrayarea;

typedef struct PcTrayareaClass
{
	GtkHBoxClass parent_class;
} PcTrayareaClass;

typedef struct PcTrayareaPrivate
{
	GtkWidget* invisible;
	Atom opcode_atom;
	Atom selection_atom;
	Atom message_data_atom;
} PcTrayareaPrivate;

G_DEFINE_TYPE(PcTrayarea, pc_trayarea, GTK_TYPE_HBOX);

#define PC_TRAYAREA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PC_TYPE_TRAYAREA, PcTrayareaPrivate))

static void pc_trayarea_unmanage(PcTrayarea* trayarea);

static void pc_trayarea_handle_dock_request(PcTrayarea* trayarea,
		XClientMessageEvent* xevent)
{
	GtkWidget* socket = gtk_socket_new();

	GdkScreen* screen = gtk_widget_get_screen(GTK_WIDGET(trayarea));
	GdkColormap* colormap = gdk_screen_get_rgb_colormap(screen);
	gtk_widget_set_colormap(socket, colormap);

	gtk_box_pack_end(GTK_BOX(trayarea), socket, FALSE, FALSE, 0);	

	gtk_widget_realize(socket);
	
	if(GTK_IS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(socket))))
	{
		gtk_socket_add_id(GTK_SOCKET(socket), xevent->data.l[2]);	
		gtk_widget_show(socket);
	}
	else
	{
		gtk_widget_destroy(socket);
	}
}

static GdkFilterReturn pc_trayarea_handle_event(PcTrayarea* trayarea,
		XClientMessageEvent* xevent)
{
	switch (xevent->data.l[1])
	{
		case SYSTEM_TRAY_REQUEST_DOCK:
			pc_trayarea_handle_dock_request(trayarea, xevent);
			return GDK_FILTER_REMOVE;

		case SYSTEM_TRAY_BEGIN_MESSAGE:
			pc_bugdialog("%s", "trayarea-plugin received event of type "
					"'SYSTEM_TRAY_BEGIN_MESSAGE'");
			return GDK_FILTER_REMOVE;

		case SYSTEM_TRAY_CANCEL_MESSAGE:
			pc_bugdialog("%s", "trayarea-plugin received event of type "
					"'SYSTEM_TRAY_CANCEL_MESSAGE'");
			return GDK_FILTER_REMOVE;

		default:
			break;
	}

	return GDK_FILTER_CONTINUE;
}

static GdkFilterReturn pc_trayarea_window_filter(GdkXEvent* xev,
		GdkEvent* event, gpointer data)
{
	XEvent* xevent = (GdkXEvent*)xev;
	PcTrayarea* trayarea = data;
	PcTrayareaPrivate* priv = PC_TRAYAREA_GET_PRIVATE(trayarea);

	if(xevent->type == ClientMessage)
	{
		if(xevent->xclient.message_type == priv->opcode_atom)
		{
			return pc_trayarea_handle_event(trayarea,
					(XClientMessageEvent*)xevent);
		}
		else if(xevent->xclient.message_type == priv->message_data_atom)
		{
			pc_bugdialog("trayarea-module received an xmessage");
			return GDK_FILTER_REMOVE;
		}
	}
	else if(xevent->type == SelectionClear)
	{
		g_warning("Trayarea lost selection-ownership");
		pc_trayarea_unmanage(trayarea);
	}

	return GDK_FILTER_CONTINUE;
}

static void pc_trayarea_unmanage(PcTrayarea* trayarea)
{
	PcTrayareaPrivate* priv = PC_TRAYAREA_GET_PRIVATE(trayarea);

	if(!priv->invisible)
		return;

	Display* display = GDK_WINDOW_XDISPLAY(priv->invisible->window);
  
	if(XGetSelectionOwner(display, priv->selection_atom) ==
			GDK_WINDOW_XWINDOW(priv->invisible->window))
    {
		guint32 timestamp = gdk_x11_get_server_time(priv->invisible->window);
		XSetSelectionOwner(display, priv->selection_atom, None, timestamp);
    }

	gdk_window_remove_filter(priv->invisible->window,
			&pc_trayarea_window_filter, trayarea);

	gtk_widget_destroy(priv->invisible);
	g_object_unref(G_OBJECT(priv->invisible));
	priv->invisible = 0;
}

static void pc_trayarea_finalize(GObject* object)
{
	PcTrayarea* trayarea = PC_TRAYAREA(object);
	pc_trayarea_unmanage(trayarea);
 
 	G_OBJECT_CLASS(pc_trayarea_parent_class)->finalize(object);
}

static void pc_trayarea_class_init(PcTrayareaClass* class)
{
	GObjectClass* obj_class = G_OBJECT_CLASS(class);
	obj_class->finalize = pc_trayarea_finalize;
	g_type_class_add_private(obj_class, sizeof(PcTrayareaPrivate));
}

static gboolean pc_trayarea_manage(PcTrayarea* trayarea)
{
	PcTrayareaPrivate* priv = PC_TRAYAREA_GET_PRIVATE(trayarea);
	Screen* xscreen = GDK_SCREEN_XSCREEN(gdk_screen_get_default());

	priv->invisible = gtk_invisible_new_for_screen(gdk_screen_get_default());
	gtk_widget_realize(priv->invisible);
	gtk_widget_add_events(priv->invisible,
			GDK_PROPERTY_CHANGE_MASK | GDK_STRUCTURE_MASK);
	gchar* selection_atom_name = g_strdup_printf("_NET_SYSTEM_TRAY_S%d",
			XScreenNumberOfScreen(xscreen));
	priv->selection_atom = XInternAtom(DisplayOfScreen(xscreen),
			selection_atom_name, False);
	g_free(selection_atom_name);

	guint32 timestamp = gdk_x11_get_server_time(priv->invisible->window);
	XSetSelectionOwner(DisplayOfScreen(xscreen), priv->selection_atom,
			GDK_WINDOW_XWINDOW(priv->invisible->window), timestamp);

	g_assert(XGetSelectionOwner(DisplayOfScreen(xscreen), priv->selection_atom)
			== GDK_WINDOW_XWINDOW(priv->invisible->window));

	XClientMessageEvent xev;
	xev.type = ClientMessage;
	xev.window = RootWindowOfScreen(xscreen);
	xev.message_type = XInternAtom(DisplayOfScreen(xscreen), "MANAGER", False);
	xev.format = 32;
	xev.data.l[0] = timestamp;
	xev.data.l[1] = priv->selection_atom;
	xev.data.l[2] = GDK_WINDOW_XWINDOW(priv->invisible->window);
	xev.data.l[3] = 0; /* manager specific data */
	xev.data.l[4] = 0; /* manager specific data */

	XSendEvent(DisplayOfScreen(xscreen),
			RootWindowOfScreen(xscreen),
			False, StructureNotifyMask, (XEvent*)&xev);

	g_object_ref(G_OBJECT(priv->invisible));

	priv->opcode_atom = XInternAtom(DisplayOfScreen(xscreen),
			"_NET_SYSTEM_TRAY_OPCODE", False);
	priv->message_data_atom = XInternAtom(DisplayOfScreen(xscreen),
			"_NET_SYSTEM_TRAY_MESSAGE_DATA", False);

	gdk_window_add_filter(priv->invisible->window,
			pc_trayarea_window_filter, trayarea);

	return TRUE;
}

static void pc_trayarea_init(PcTrayarea* trayarea)
{
	pc_trayarea_manage(trayarea);
}

GtkWidget* pc_trayarea_new()
{
	return GTK_WIDGET(g_object_new(PC_TYPE_TRAYAREA, NULL));
}

static GtkWidget* pc_trayarea_instantiate(Config* config)
{
	return pc_trayarea_new();
}

static ConfigOption pc_trayarea_options[] = {
	CFG_END()
};

static const PcWidgetInfo pc_trayarea_info = {
	.name = "trayarea",
	.instantiate = &pc_trayarea_instantiate,
	.options = pc_trayarea_options
};

gboolean pc_module_init(const PcModuleCallbacks* callbacks)
{
	callbacks->register_widget(&pc_trayarea_info);
	return TRUE;
}


