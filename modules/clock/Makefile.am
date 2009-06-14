INCLUDES = \
	-I$(top_srcdir)/lib \
	$(gtk_CFLAGS)

pkglib_LTLIBRARIES = libclock.la

libclock_la_SOURCES = pc_clock.c

libclock_la_LDFLAGS = \
	$(gtk_LIBS) \
	$(top_builddir)/lib/libpancake.la \
	-rdynamic

