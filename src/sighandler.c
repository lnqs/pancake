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

#define _POSIX_SOURCE

#include "sighandler.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

static gint sigpipe[2];
static GIOChannel* io_channel;

static const gint signals_to_catch[] =
{
	SIGHUP,
	SIGINT,
	SIGQUIT,
	SIGUSR1,
	SIGUSR2,
	SIGPIPE,
	SIGALRM,
	SIGTERM,
	SIGIO,
	SIGXCPU,
	SIGXFSZ,
	SIGVTALRM,
	SIGPROF,
	0
};

static void pc_sighandler_unix_sighandler(int signum)
{
	if(write(sigpipe[1], &signum, sizeof(int)) != sizeof(int))
		g_print("Failed to write signal %i to signalpipe\n", signum);
}

static gboolean pc_sighandler_final_sighandler(
		GIOChannel* source, GIOCondition cond, gpointer d)
{
	GError* error = NULL;
	GIOStatus status;
	gsize read;
	gint signum;

	/* Since we always just shutdown, we clear the pipe, but ignore
	   the data received */
	while((status = g_io_channel_read_chars(source, (gchar*)&signum,
			sizeof(gint), &read, &error)) == G_IO_STATUS_NORMAL);
  
	if(error != NULL)
	{
		g_print("failed to read from signalpipe: %s\n", error->message);
		g_error_free(error);
	}

	if(status == G_IO_STATUS_EOF)
		g_print("signalpipe closed\n");

	gtk_main_quit();

	return FALSE;
}

static gboolean pc_sighandler_create_sigpipe()
{
	/* create pipe */
	if(pipe(sigpipe))
	{
		g_print("FATAL: failed to create signalpipe: %s\n", strerror(errno));
		return FALSE;
	}

	/* set non-blocking */
	gint flags = fcntl(sigpipe[1], F_GETFL);
	if(flags == -1)
	{
		g_print("FATAL: failed read signalpipe's flags: %s\n", strerror(errno));
		return FALSE;
	}

	if(fcntl(sigpipe[1], F_SETFL, flags | O_NONBLOCK) == -1)
	{
		g_print("FATAL: failed set signalpipe's flags: %s\n", strerror(errno));
		return FALSE;
	}

	return TRUE;
}

static gboolean pc_sighandler_register_signals()
{
	sigset_t mask;
	if(sigfillset(&mask))
	{
		g_print("FATAL: failed create signalmask: %s\n", strerror(errno));
		return FALSE;
	}
	
	struct sigaction act =
	{
		.sa_handler = &pc_sighandler_unix_sighandler,
		.sa_mask = mask,
		.sa_flags = 0,
		.sa_restorer = NULL
	};
	
	for(int i = 0; signals_to_catch[i]; i++)
	{
		if(sigaction(signals_to_catch[i], &act, NULL))
		{
			g_print("FATAL: failed set signalhandler: %s\n", strerror(errno));
			return FALSE;
		}
	}
	
	return TRUE;
}

gboolean pc_sighandler_init()
{
	GError* error = 0;
	
	if(!pc_sighandler_create_sigpipe() || !pc_sighandler_register_signals())
		return FALSE;

	io_channel = g_io_channel_unix_new(sigpipe[0]);

	g_io_channel_set_encoding(io_channel, NULL, &error);
	if(error)
	{
		g_print("FATAL: failed to set GIOChannel-encoding for "
				"signalhandling: %s\n", error->message);
		g_error_free(error);
		return FALSE;
	}

	g_io_channel_set_flags(io_channel,
			g_io_channel_get_flags(io_channel) | G_IO_FLAG_NONBLOCK, &error);
	if(error)
	{
		g_print("FATAL: failed to set set sigpipe in non-blocking-mode: %s",
				error->message);
		g_error_free(error);
		return FALSE;
	}

	g_io_add_watch(io_channel, G_IO_IN | G_IO_PRI,
			&pc_sighandler_final_sighandler, NULL);
	
	return TRUE;
}

