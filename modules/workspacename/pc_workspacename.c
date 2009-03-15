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

#define PC_TYPE_WORKSPACENAME	(pc_workspacename_get_type())
#define PC_WORKSPACENAME(obj) \
		(G_TYPE_CHECK_INSTANCE_CAST((obj), \
		PC_TYPE_WORKSPACENAME, PcWorkspacename))
#define PC_WORKSPACENAME_CLASS(obj) \
		(G_TYPE_CHECK_CLASS_CAST((obj), PC_WORKSPACENAME, PcWorkspacenameClass))
#define PC_IS_WORKSPACENAME(obj) \
		(G_TYPE_CHECK_INSTANCE_TYPE((obj), PC_TYPE_WORKSPACENAME))
#define PC_IS_WORKSPACENAME_CLASS(obj) \
		(G_TYPE_CHECK_CLASS_TYPE((obj), PC_TYPE_WORKSPACENAME))
#define PC_WORKSPACENAME_GET_CLASS \
		(G_TYPE_INSTANCE_GET_CLASS((obj), \
		PC_TYPE_WORKSPACENAME, PcWorkspacenameClass))

typedef struct PcWorkspacename
{
	GtkLabel parent;
} PcWorkspacename;

typedef struct PcWorkspacenameClass
{
	GtkLabelClass parent_class;
} PcWorkspacenameClass;

G_DEFINE_TYPE(PcWorkspacename, pc_workspacename, GTK_TYPE_LABEL);


#define PC_WORKSPACENAME_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE((obj), \
		PC_TYPE_WORKSPACENAME, PcWorkspacenamePrivate))

static void pc_workspacename_active_workspace_changed(WnckScreen* wscr,
		WnckWorkspace* previously_active_space, gpointer data)
{
	gtk_label_set_text(GTK_LABEL(data), wnck_workspace_get_name(
			wnck_screen_get_active_workspace(wnck_screen_get_default())));
}

static void pc_workspacename_class_init(PcWorkspacenameClass* class)
{
}

static void pc_workspacename_init(PcWorkspacename* workspacename)
{
	g_signal_connect(G_OBJECT(wnck_screen_get_default()),
			"active-workspace-changed",
			G_CALLBACK(&pc_workspacename_active_workspace_changed),
			workspacename);
}

static GtkWidget* pc_workspacename_new()
{
	return GTK_WIDGET(g_object_new(PC_TYPE_WORKSPACENAME, NULL));
}

static PancakePlugin pc_workspacename = {
	.name       = "workspacename",
	.init       = NULL,
	.fini       = NULL,
	.new_widget = &pc_workspacename_new
};

PANCAKE_PLUGIN(pc_workspacename)

