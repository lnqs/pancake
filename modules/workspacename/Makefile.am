INCLUDES = \
	-I$(top_srcdir)/lib \
	$(gtk_CFLAGS) \
	$(libwnck_CFLAGS)

pkglib_LTLIBRARIES = libworkspacename.la

libworkspacename_la_SOURCES = \
	pc_workspacename.c

libworkspacename_la_LDFLAGS = \
	$(gtk_LIBS) \
	$(libwnck_LIBS) \
	$(top_srcdir)/lib/libpancake.la \
	-rdynamic

