AC_PREREQ(2.59)
AC_INIT([Pancake], [0.0.1])
AM_INIT_AUTOMAKE([Pancake], [0.0.1])
AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([foreign dist-bzip2])

AC_PROG_CXX
AC_PROG_LIBTOOL

AC_DEFINE(PACKAGE_BUGREPORT, "simon.schoenfeld@web.de")

CFLAGS="$CPPFLAGS -Wall -Werror -std=c99 -g"

### Check for dependencies
# GTK+-2
PKG_CHECK_MODULES(gtk, [gtk+-2.0 >= 2.12.0],,
	AC_MSG_ERROR([gtk+-2.0 >= 2.12.0 not found!]))

# libwnck
PKG_CHECK_MODULES(libwnck, [libwnck-1.0 >= 2.22.0],,
	AC_MSG_ERROR([libwnck-1.0 >= 2.20.0 not found!]))

# libconfuse
PKG_CHECK_MODULES(libconfuse, [libconfuse >= 2.6],,
	AC_MSG_ERROR([libconfuse >= 2.6 not found!]))

### Write Makefiles
AC_SUBST(gtk_CFLAGS)
AC_SUBST(gtk_LIBS)
AC_SUBST(libwnck_CFLAGS)
AC_SUBST(libwnck_LIBS)
AC_SUBST(libconfuse_CFLAGS)
AC_SUBST(libconfuse_LIBS)

AC_CONFIG_FILES([ \
	Makefile \
	lib/Makefile \
	src/Makefile \
	modules/Makefile \
	modules/tasklist/Makefile \
	modules/clock/Makefile \
	modules/pager/Makefile \
	modules/trayarea/Makefile \
	modules/defaulttheme/Makefile \
	pancake.pc
])

AC_OUTPUT
