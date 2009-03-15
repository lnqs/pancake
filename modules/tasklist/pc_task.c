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

#include "pc_task.h"

G_DEFINE_TYPE(PcTask, pc_task, GTK_TYPE_TOGGLE_BUTTON);

enum
{
	PROP_0,
	PROP_WNCK_WINDOW,
	PROP_LAST
};

#define PC_TASK_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE((obj), PC_TYPE_TASK, PcTaskPrivate))

typedef struct PcTaskPrivate
{
	WnckWindow* wwin;
	GtkWidget* wnck_action_menu;
} PcTaskPrivate;

static void pc_task_update_state(PcTask* task)
{
	PcTaskPrivate* priv = PC_TASK_GET_PRIVATE(task);

	if(wnck_window_is_active(priv->wwin))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(task), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(task), FALSE);
}

static gboolean pc_task_determine_visibility(PcTask* task)
{
	PcTaskPrivate* priv = PC_TASK_GET_PRIVATE(task);

	if(wnck_window_is_skip_tasklist(priv->wwin))
		return FALSE;

	if(wnck_window_is_pinned(priv->wwin))
		return TRUE;
	
	WnckWorkspace* active_workspace = wnck_screen_get_active_workspace(
			wnck_screen_get_default());

	if(!active_workspace)
		return TRUE;
	
	if(wnck_window_get_workspace(priv->wwin) != active_workspace)
		return FALSE;
	
	if(!wnck_workspace_is_virtual(active_workspace))
		return TRUE;

	if(wnck_window_is_in_viewport(priv->wwin, active_workspace))
		return TRUE;
	
	return FALSE;
}

static void pc_task_update_visibility(PcTask* task)
{
	if(pc_task_determine_visibility(task))
		gtk_widget_show(GTK_WIDGET(task));
	else
		gtk_widget_hide(GTK_WIDGET(task));
}


static void pc_task_active_window_changed(WnckScreen* wscr,
		WnckWindow* previously_active_window, gpointer data)
{
	pc_task_update_state(PC_TASK(data));
}

static void pc_task_active_workspace_changed(WnckScreen* wscr,
		WnckWorkspace* previously_active_space, gpointer data)
{
	pc_task_update_visibility(PC_TASK(data));
}

static void pc_task_name_changed(WnckWindow* wwin, gpointer data)
{
	gtk_button_set_label(GTK_BUTTON(data), wnck_window_get_name(wwin));
}

static void pc_task_workspace_changed(WnckWindow* wwin, gpointer data)
{
	pc_task_update_visibility(PC_TASK(data));
}

static void pc_button_release_event(GtkWidget* widget,
		GdkEventButton* event, gpointer data)
{
	PcTaskPrivate* priv = PC_TASK_GET_PRIVATE(widget);

	if(event->button == 1)
		wnck_window_activate(priv->wwin, event->time);
	else if(event->button == 3)
		gtk_menu_popup(GTK_MENU(priv->wnck_action_menu),
				NULL, NULL, NULL, NULL, event->button, event->time);
}

static void pc_task_set_property(GObject* object,
		guint prop_id, const GValue* value, GParamSpec* pspec)
{
	PcTaskPrivate* priv = PC_TASK_GET_PRIVATE(object);
	
	switch(prop_id)
	{
		case PROP_WNCK_WINDOW:
			priv->wwin = g_value_get_pointer(value);
			break;

		default:
	    	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
    }
}

static void pc_task_get_property(GObject* object,
		guint prop_id, GValue* value, GParamSpec* pspec)
{
	PcTaskPrivate* priv = PC_TASK_GET_PRIVATE(object);

	switch(prop_id)
	{
		case PROP_WNCK_WINDOW:
			g_value_set_pointer(value, priv->wwin);
			break;
	
		default:
	    	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
    }
}

static void pc_task_finalize(GObject* object)
{
	guint num_disconnected;
	
	num_disconnected = g_signal_handlers_disconnect_matched(
			G_OBJECT(wnck_window_get_screen(PC_TASK_GET_PRIVATE(object)->wwin)),
			G_SIGNAL_MATCH_DATA, 0, 0, 0, 0, object);
	g_assert(num_disconnected == 2);

	num_disconnected = g_signal_handlers_disconnect_matched(
			G_OBJECT(PC_TASK_GET_PRIVATE(object)->wwin),
			G_SIGNAL_MATCH_DATA, 0, 0, 0, 0, object);
	g_assert(num_disconnected == 2);

	gtk_widget_destroy(PC_TASK_GET_PRIVATE(object)->wnck_action_menu);
}

static void pc_task_class_init(PcTaskClass* class)
{
	GObjectClass* obj_class = G_OBJECT_CLASS(class);

	obj_class->set_property = &pc_task_set_property;
	obj_class->get_property = &pc_task_get_property;
	obj_class->finalize = &pc_task_finalize;

	g_object_class_install_property(obj_class, PROP_WNCK_WINDOW,
			g_param_spec_pointer("wnckwindow",
					"WnckWindow instance",
					"WnckWindow with this tasks data.",
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_type_class_add_private(obj_class, sizeof(PcTaskPrivate));
}

static void pc_task_init(PcTask* task)
{
}

GtkWidget* pc_task_new(WnckWindow* wwin)
{
	/* TODO: It sucks to have this logic in this function, is there is better
	         way? */
	GtkWidget* task = GTK_WIDGET(g_object_new(PC_TYPE_TASK, "wnckwindow",
			wwin, "label", wnck_window_get_name(wwin), NULL));

	GtkWidget* label = gtk_bin_get_child(GTK_BIN(task));
	gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);

	g_signal_connect(G_OBJECT(wnck_window_get_screen(wwin)),
			"active-window-changed",
			G_CALLBACK(&pc_task_active_window_changed), task);
	g_signal_connect(G_OBJECT(wnck_window_get_screen(wwin)),
			"active-workspace-changed",
			G_CALLBACK(&pc_task_active_workspace_changed), task);
	g_signal_connect(G_OBJECT(wwin), "name-changed",
			G_CALLBACK(&pc_task_name_changed), task);
	g_signal_connect(G_OBJECT(wwin), "workspace-changed",
			G_CALLBACK(&pc_task_workspace_changed), task);
	g_signal_connect(G_OBJECT(task), "button-release-event",
			G_CALLBACK(&pc_button_release_event), NULL);

	PC_TASK_GET_PRIVATE(task)->wnck_action_menu = wnck_action_menu_new(wwin);

	pc_task_update_visibility(PC_TASK(task));

	return task;
}

WnckWindow* pc_task_get_wnck_window(PcTask* task)
{
	PcTaskPrivate* priv = PC_TASK_GET_PRIVATE(task);
	return priv->wwin;
}

