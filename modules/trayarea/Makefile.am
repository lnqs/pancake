INCLUDES = \
	-I$(top_srcdir)/lib \
	$(gtk_CFLAGS)

pkglib_LTLIBRARIES = libtrayarea.la

libtrayarea_la_SOURCES = pc_trayarea.c

libtrayarea_la_LDFLAGS = \
	$(gtk_LIBS) \
	$(top_builddir)/lib/libpancake.la \
	-rdynamic

