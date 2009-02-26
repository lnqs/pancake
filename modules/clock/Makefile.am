INCLUDES = \
	-I$(top_srcdir)/src \
	$(gtk_CFLAGS)

pkglib_LTLIBRARIES = libclock.la

libclock_la_SOURCES = pc_clock.c

libclock_la_LDFLAGS = \
	$(gtk_LIBS) \
	-rdynamic

