INCLUDES = \
	$(gtk_CFLAGS) \
	$(libwnck_CFLAGS)

pancakeincludedir = $(includedir)/pancake

bin_PROGRAMS = pancake

noinst_HEADERS = \
	sighandler.h \
	commandline.h \
	modloader.h \
	configparser.h \
	panel.h \
	types.h

pancakeinclude_HEADERS = \
	module.h

pancake_SOURCES = \
	main.c \
	sighandler.c \
	commandline.c \
	modloader.c \
	configparser.c \
	panel.c

pancake_CFLAGS = \
	-DMODULE_PATH="\"$(libdir)/pancake\""

pancake_LDFLAGS = \
	$(gtk_LIBS) \
	$(libwnck_LIBS)

