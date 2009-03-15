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

#ifndef PC_PANEL_H
#define PC_PANEL_H

#include <gtk/gtk.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

G_BEGIN_DECLS

#define PC_TYPE_TASK			(pc_task_get_type())
#define PC_TASK(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), PC_TYPE_TASK, PcTask))
#define PC_TASK_CLASS(obj)		(G_TYPE_CHECK_CLASS_CAST((obj), PC_TASK, PcTaskClass))
#define PC_IS_TASK(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), PC_TYPE_TASK))
#define PC_IS_TASK_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE((obj), PC_TYPE_TASK))
#define PC_TASK_GET_CLASS		(G_TYPE_INSTANCE_GET_CLASS((obj), PC_TYPE_TASK, PcTaskClass))

/**
 * @todo docs
 **/
typedef struct PcTask
{
	GtkToggleButton parent;
} PcTask;

/**
 * @todo docs
 **/
typedef struct PcTaskClass
{
	GtkToggleButtonClass parent_class;
} PcTaskClass;

/**
 * @todo docs
 **/
GtkWidget* pc_task_new(WnckWindow* wwin);

/**
 * @todo docs
 **/
WnckWindow* pc_task_get_wnck_window(PcTask* task);

/**
 * @todo docs
 **/
GType pc_task_get_type();

G_END_DECLS

#endif

