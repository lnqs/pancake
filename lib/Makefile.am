INCLUDES = \
	$(gtk_CFLAGS) \
	$(libconfuse_CFLAGS)

libpancakeincludedir = $(includedir)/pancake

lib_LTLIBRARIES = libpancake.la

libpancakeinclude_HEADERS = \
	pc_module.h \
	pc_misc.h \
	pc_style.h \
	pc_types.h

libpancake_la_SOURCES = \
	pc_style.c \
	pc_misc.c

libpancake_la_LDFLAGS = \
	$(gtk_LIBS) \
	$(libconfuse_LIBS) \
	-rdynamic

