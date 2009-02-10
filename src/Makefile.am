INCLUDES = \
	$(gtk_CFLAGS) \
	$(libwnck_CFLAGS)

pancakeincludedir = $(includedir)/pancake

bin_PROGRAMS = pancake

noinst_HEADERS = \
	pc_sighandler.h \
	pc_commandline.h \
	pc_modloader.h \
	pc_configparser.h \
	pc_panel.h \
	pc_types.h

pancakeinclude_HEADERS = \
	pc_types.h \
	pc_misc.h \
	pc_module.h \
	pc_optionconv.h

pancake_SOURCES = \
	main.c \
	pc_sighandler.c \
	pc_commandline.c \
	pc_modloader.c \
	pc_configparser.c \
	pc_panel.c \
	pc_optionconv.c

pancake_CFLAGS = \
	-DPC_MODULE_PATH="\"$(libdir)/@PACKAGE@\""

pancake_LDFLAGS = \
	$(gtk_LIBS) \
	$(libwnck_LIBS)

