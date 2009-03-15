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
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include "pc_task.h"

#define PC_TYPE_TASKLIST (pc_tasklist_get_type())
#define PC_TASKLIST(obj) \
		(G_TYPE_CHECK_INSTANCE_CAST((obj), PC_TYPE_TASKLIST, PcTasklist))
#define PC_TASKLIST_CLASS(obj) \
		(G_TYPE_CHECK_CLASS_CAST((obj), PC_TASKLIST, PcTasklistClass))
#define PC_IS_TASKLIST(obj) \
		(G_TYPE_CHECK_INSTANCE_TYPE((obj), PC_TYPE_TASKLIST))
#define PC_IS_TASKLIST_CLASS(obj) \
		(G_TYPE_CHECK_CLASS_TYPE((obj), PC_TYPE_TASKLIST))
#define PC_TASKLIST_GET_CLASS \
		(G_TYPE_INSTANCE_GET_CLASS((obj), PC_TYPE_TASKLIST, PcTasklistClass))

typedef struct PcTasklist
{
	GtkHBox parent;
} PcTasklist;

typedef struct PcTasklistClass
{
	GtkHBoxClass parent_class;
} PcTasklistClass;

G_DEFINE_TYPE(PcTasklist, pc_tasklist, GTK_TYPE_HBOX);

#define PC_TASKLIST_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), PC_TYPE_TASKLIST, PcTasklistPrivate))

static void pc_tasklist_window_opened(WnckScreen* wscr,
		WnckWindow* wwin, gpointer data)
{
	PcTasklist* tasklist = PC_TASKLIST(data);

	GtkWidget* task = pc_task_new(wwin);
	pc_style_apply(gtk_widget_get_style(GTK_WIDGET(tasklist)), GTK_WIDGET(task));
	gtk_box_pack_end(GTK_BOX(tasklist), task, FALSE, TRUE, 0);
}

static void pc_tasklist_delete_child_if_wwin(GtkWidget* widget, gpointer data)
{
	if(PC_IS_TASK(widget) && pc_task_get_wnck_window(PC_TASK(widget)) == data)
		gtk_widget_destroy(widget);
}

static void pc_tasklist_window_closed(WnckScreen* wscr,
		WnckWindow* wwin, gpointer data)
{
	PcTasklist* tasklist = PC_TASKLIST(data);

	gtk_container_foreach(GTK_CONTAINER(tasklist),
			&pc_tasklist_delete_child_if_wwin, wwin);
}

static void pc_tasklist_class_init(PcTasklistClass* class)
{
}

static void pc_tasklist_init(PcTasklist* tasklist)
{
	GTK_BOX(tasklist)->spacing = 0;
	GTK_BOX(tasklist)->homogeneous = TRUE;

	g_signal_connect(G_OBJECT(wnck_screen_get_default()), "window_opened",
			G_CALLBACK(pc_tasklist_window_opened), tasklist);
	g_signal_connect(G_OBJECT(wnck_screen_get_default()), "window_closed",
			G_CALLBACK(pc_tasklist_window_closed), tasklist);
}

static GtkWidget* pc_tasklist_new()
{
	return GTK_WIDGET(g_object_new(PC_TYPE_TASKLIST, NULL));
}

static PancakePlugin pc_tasklist = {
	.name       = "tasklist",
	.init       = NULL,
	.fini       = NULL,
	.new_widget = &pc_tasklist_new
};

PANCAKE_PLUGIN(pc_tasklist)

