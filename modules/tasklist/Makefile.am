INCLUDES = \
	-I$(top_srcdir)/lib \
	$(gtk_CFLAGS) \
	$(libwnck_CFLAGS)

pkglib_LTLIBRARIES = libtasklist.la

libtasklist_la_SOURCES = \
	pc_tasklist.c

libtasklist_la_LDFLAGS = \
	$(gtk_LIBS) \
	$(libwnck_LIBS) \
	$(top_builddir)/lib/libpancake.la \
	-rdynamic

