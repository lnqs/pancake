INCLUDES = \
	-I$(top_srcdir)/lib \
	$(gtk_CFLAGS) \
	$(libwnck_CFLAGS) \
	$(libconfuse_CFLAGS)

bin_PROGRAMS = pancake

noinst_HEADERS = \
	pc_sighandler.h \
	pc_commandline.h \
	pc_modloader.h \
	pc_configparser.h \
	pc_panel.h

pancake_SOURCES = \
	main.c \
	pc_sighandler.c \
	pc_commandline.c \
	pc_modloader.c \
	pc_configparser.c \
	pc_panel.c

pancake_CFLAGS = \
	-DPC_MODULE_PATH="\"$(libdir)/@PACKAGE@\""

pancake_LDFLAGS = \
	$(gtk_LIBS) \
	$(libwnck_LIBS) \
	$(libconfuse_LIBS) \
	$(top_builddir)/lib/libpancake.la

