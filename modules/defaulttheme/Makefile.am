INCLUDES = \
	-I$(top_srcdir)/lib \
	$(gtk_CFLAGS)

pkglib_LTLIBRARIES = libdefaulttheme.la

libdefaulttheme_la_SOURCES = pc_defaulttheme.c

libdefaulttheme_la_LDFLAGS = \
	$(gtk_LIBS) \
	$(top_srcdir)/lib/libpancake.la \
	-rdynamic 

