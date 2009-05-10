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

#include <pc_module.h>

#define PC_TYPE_CLOCK	(pc_clock_get_type())
#define PC_CLOCK(obj) \
		(G_TYPE_CHECK_INSTANCE_CAST((obj), PC_TYPE_CLOCK, PcClock))
#define PC_CLOCK_CLASS(obj) \
		(G_TYPE_CHECK_CLASS_CAST((obj), PC_CLOCK, PcClockClass))
#define PC_IS_CLOCK(obj) \
		(G_TYPE_CHECK_INSTANCE_TYPE((obj), PC_TYPE_CLOCK))
#define PC_IS_CLOCK_CLASS(obj) \
		(G_TYPE_CHECK_CLASS_TYPE((obj), PC_TYPE_CLOCK))
#define PC_CLOCK_GET_CLASS \
		(G_TYPE_INSTANCE_GET_CLASS((obj), PC_TYPE_CLOCK, PcClockClass))

typedef struct PcClock
{
	GtkLabel parent;
	gchar* format;
} PcClock;

typedef struct PcClockClass
{
	GtkLabelClass parent_class;
} PcClockClass;

G_DEFINE_TYPE(PcClock, pc_clock, GTK_TYPE_LABEL);

enum
{
	PROP_0,
	PROP_FORMAT,
	PROP_LAST
};

#define PC_CLOCK_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE((obj), PC_TYPE_CLOCK, PcClockPrivate))

static gboolean pc_clock_update(gpointer data)
{
	PcClock* clock = data;

	if(!clock->format)
		return TRUE;

	char buf[64];
	time_t tt = time(NULL);
	struct tm* tm = localtime(&tt);
	if(strftime(buf, sizeof(buf), clock->format, tm))
		gtk_label_set_text(GTK_LABEL(clock), buf);

	return TRUE;
}

static void pc_clock_set_property(GObject* object,
		guint prop_id, const GValue* value, GParamSpec* pspec)
{
	switch(prop_id)
	{
		case PROP_FORMAT:
			if(g_value_get_string(value))
			{
				g_free(PC_CLOCK(object)->format);
	    		PC_CLOCK(object)->format = g_strdup(g_value_get_string(value));
				pc_clock_update(object);
			}
			break;
	
		default:
	    	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
    }
}

static void pc_clock_get_property(GObject* object,
		guint prop_id, GValue* value, GParamSpec* pspec)
{
	switch(prop_id)
	{
		case PROP_FORMAT:
			g_value_set_string(value, PC_CLOCK(object)->format);
			break;
	
		default:
	    	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
    }
}

static void pc_clock_class_init(PcClockClass* class)
{
	GObjectClass* obj_class = G_OBJECT_CLASS(class);

	obj_class->set_property = pc_clock_set_property;
	obj_class->get_property = pc_clock_get_property;

	g_object_class_install_property(obj_class, PROP_FORMAT,
			g_param_spec_string("format",
					"Display format",
					"The format to show the time in. Default: %H:%M",
					"%H:%M",
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void pc_clock_init(PcClock* clock)
{
	if(!clock->format)
		clock->format = g_strdup("%H:%M");

	g_timeout_add_seconds(1, pc_clock_update, clock);
}

static GtkWidget* pc_clock_new()
{
	return GTK_WIDGET(g_object_new(PC_TYPE_CLOCK, NULL));
}

static GtkWidget* pc_clock_instantiate(cfg_t* config)
{
	GtkWidget* widget = pc_clock_new();
	GValue value = { 0, };
	g_value_init(&value, G_TYPE_STRING);

	const gchar* format = cfg_getstr(config, "format");
	g_value_set_string(&value, format);
	g_object_set_property(G_OBJECT(widget), "format", &value);
	g_value_unset(&value);

	return widget;
}

static cfg_opt_t pc_clock_options[] = {
	CFG_STR("format", "%H:%M", CFGF_NONE),
	CFG_END()
};

static const PcWidgetInfo pc_clock_info = {
	.name = "clock",
	.instantiate = &pc_clock_instantiate,
	.options = pc_clock_options
};

gboolean pc_module_init(const PcModuleCallbacks* callbacks)
{
	callbacks->register_widget(&pc_clock_info);
	return TRUE;
}

