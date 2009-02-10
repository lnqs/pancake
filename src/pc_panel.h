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
#include "pc_types.h"

G_BEGIN_DECLS

#define PC_TYPE_PANEL			(pc_panel_get_type())
#define PC_PANEL(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), PC_TYPE_PANEL, PcPanel))
#define PC_PANEL_CLASS(obj)		(G_TYPE_CHECK_CLASS_CAST((obj), PC_PANEL, PcPanelClass))
#define PC_IS_PANEL(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), PC_TYPE_PANEL))
#define PC_IS_PANEL_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE((obj), PC_TYPE_PANEL))
#define PC_PANEL_GET_CLASS		(G_TYPE_INSTANCE_GET_CLASS((obj), PC_TYPE_PANEL, PcPanelClass))

/**
 * @brief pancake-panel-widget
 **/
typedef struct PcPanel
{
	GtkWindow parent;
} PcPanel;

/**
 * @brief pancake-panel-widget-class
 **/
typedef struct PcPanelClass
{
	GtkWindowClass parent_class;
} PcPanelClass;

/**
 * @brief Creates a new pancake-panel
 *
 * @return the newly created panel
 **/
GtkWidget* pc_panel_new();

/**
 * @brief Sets the width of a pancake-panel
 *
 * @param panel the panel to set the width
 * @param width the width relativ to the screen's width, in range 0..1
 **/
void pc_panel_set_width(PcPanel* panel, gfloat width);

/**
 * @brief Sets the height of a pancake-panel
 *
 * @param panel the panel to set the height
 * @param height the height in pixels
 **/
void pc_panel_set_height(PcPanel* panel, gint height);

/**
 * @brief Sets the alignment of a pancake-panel
 *
 * @param panel the panel to set the alignment
 * @param align the alignment
 **/
void pc_panel_set_align(PcPanel* panel, PcAlignment align);

/**
 * @brief Sets if a panel should set a strut
 *
 * @param panel the panel to enable/disable the strut
 * @param enabled TRUE to enable, FALSE to disable
 **/
void pc_panel_set_strut_enabled(PcPanel* panel, gboolean enabled);

/**
 * @brief Sets an panel-option by a key
 *
 * @param key the option's key
 * @param value the option's value
 * @return TRUE on success, FALSE otherwise
 **/
gboolean pc_panel_set_option(
		PcPanel* panel, const gchar* key, const gchar* value);

G_END_DECLS

#endif

