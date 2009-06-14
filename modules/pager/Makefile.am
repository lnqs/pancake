INCLUDES = \
	-I$(top_srcdir)/lib \
	$(gtk_CFLAGS) \
	$(libwnck_CFLAGS)

pkglib_LTLIBRARIES = libpager.la

libpager_la_SOURCES = \
	pc_pager.c

libpager_la_LDFLAGS = \
	$(gtk_LIBS) \
	$(libwnck_LIBS) \
	$(top_builddir)/lib/libpancake.la \
	-rdynamic

