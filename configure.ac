dnl Set package name and maintainer
AC_INIT([eiciel],[0.9.13.1],[rofirrim@gmail.com])

dnl Where are we
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Require autoconf 2.59
AC_PREREQ(2.59)

dnl Initialize automake
AM_INIT_AUTOMAKE([1.11 dist-bzip2 foreign -Wportability])

AM_SILENT_RULES([yes])

dnl gettext support
GETTEXT_PACKAGE=eiciel
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [gettext domain])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18.1])

dnl m4 files directory
AC_CONFIG_MACRO_DIR([m4])

dnl Source check
AC_CONFIG_SRCDIR(src/eiciel_standalone.cpp)

dnl Config header name
AC_CONFIG_HEADER(config.hpp)

dnl We want tests in C++
AC_LANG(C++)

dnl Check install program
AC_PROG_INSTALL

dnl Check cpp
AC_PROG_CPP

dnl Check c++ compiler
AC_PROG_CXX

AX_CXX_COMPILE_STDCXX_11([ext],[optional])

dnl Check libtool
LT_INIT

dnl GNOME 2 or 3?
AC_ARG_WITH(gnome-version,
AS_HELP_STRING([--with-gnome-version=VERSION],[GNOME major version (2, 3, or check; default is check)])
,
[
if test x$withval != x2 -a x$withval != x3 -a x$withval != xcheck;
then
  AC_MSG_ERROR([Valid choices for --with-gnome-version= are 2, 3, and check.])
fi
]
,
[with_gnome_version=check])

if test x$with_gnome_version = xcheck;
then
  PKG_CHECK_EXISTS(gtkmm-3.0 >= 3.0.0,
  [with_gnome_version=3],
  [with_gnome_version=2])
else
  dnl If we don't do this, $PKG_CONFIG can end up undefined when $with_gnome_version != check
  PKG_PROG_PKG_CONFIG
fi

dnl Check if we want to build nautilus extension
AC_ARG_ENABLE([nautilus-extension],
  [AS_HELP_STRING([--enable-nautilus-extension], [Build nautilus extension (default is yes)])],
  [],
  [enable_nautilus_extension=yes])
AM_CONDITIONAL([ENABLE_NAUTILUS_EXTENSION], [test "x$enable_nautilus_extension" = "xyes"])

AH_TEMPLATE([USING_GNOME2],[Define to 1 if building eiciel for GNOME 2 (instead of GNOME 3)])

AS_IF([test "x$with_gnome_version" = "x3"], [
dnl Check for gtkmm-3 and dependencies from GNOME 3 ...
  AC_MSG_NOTICE([checking for dependencies for GNOME 3 version of eiciel])
  PKG_CHECK_MODULES(GTKMM, gtkmm-3.0 >= 3.0.0 giomm-2.4 >= 2.50.0)
  AM_COND_IF([ENABLE_NAUTILUS_EXTENSION], [PKG_CHECK_MODULES(NAUTILUS_EXTENSION, libnautilus-extension >= 3.0.0)])
], [
dnl ... or check for gtkmm-2.4 and dependencies from GNOME 2
  AC_MSG_NOTICE([checking for dependencies for GNOME 2 version of eiciel])
  PKG_CHECK_MODULES(GTKMM, gtkmm-2.4 >= 2.4.0 libgnome-2.0 >= 2.10.0 giomm-2.4 >= 2.50.0, [AC_DEFINE(USING_GNOME2, 1)])
  AM_COND_IF([ENABLE_NAUTILUS_EXTENSION], [PKG_CHECK_MODULES(NAUTILUS_EXTENSION, libnautilus-extension >= 2.10.0 libnautilus-extension < 2.90)])
])

AM_COND_IF([ENABLE_NAUTILUS_EXTENSION], [
AC_MSG_CHECKING([for nautilus extensions directory])

AC_ARG_WITH(nautilus-extensions-dir, 
AS_HELP_STRING([--with-nautilus-extensions-dir=DIR],[Directory where nautilus extensions have to be installed (usually /usr/lib/nautilus/extensions-2.0 or -3.0)])
, 
[
if test x$withval = xyes -o x$withval = xno;
then
   AC_MSG_ERROR([If you specify --with-nautilus-extensions-dir you must set the directory. E.g.: --with-nautilus-extensions-dir=/usr/lib/nautilus/extensions-2.0])
else 
 AC_SUBST(NAUTILUS_EXTENSIONS_DIR, [$withval])
 AC_MSG_RESULT([$withval])
fi
]
,
[dnl Linux distributions
  extensiondir=`pkg-config --variable=extensiondir libnautilus-extension`;
  if test -n "$extensiondir" ;
  then
     AC_SUBST(NAUTILUS_EXTENSIONS_DIR, [$extensiondir])
	 AC_MSG_RESULT(${NAUTILUS_EXTENSIONS_DIR})
  else
   	AC_MSG_ERROR([I need pkg-config in order to determine where to install nautilus extensions. You may want to set --with-nautilus-extensions-dir=DIR])
  fi
]
)
])

enable_eua=no
AC_MSG_CHECKING([for extended user attributes support])
AC_ARG_ENABLE(user-attributes,
AS_HELP_STRING([--enable-user-attributes],[Enables support for extended user attributes. This is GNU/Linux specific.]),
[
if test x$enableval = xyes -o x$enableval = x;
then
   AC_MSG_RESULT([yes])
   enable_eua=yes
else
  if test x$enableval = xno;
  then
    enable_eua=no
    AC_MSG_RESULT([no])
  else
    AC_MSG_ERROR([This option can only be given 'yes' or 'no' values])
  fi
fi
]
,
[
 case $target_os in
     linux*)
	   enable_eua=yes
       AC_MSG_RESULT([yes, since we are in GNU/Linux])
	   ;;
	 *)
	   enable_eua=no
	   AC_MSG_RESULT([no, this does not seem to be GNU/Linux])
	 ;;
 esac
]
)

if test x$enable_eua = xyes;
then
    AC_CHECK_HEADERS([sys/xattr.h], [], AC_MSG_ERROR([This header is mandatory for extended user attributes support]), [])
    AC_DEFINE([ENABLE_USER_XATTR], [1], [Enables user extended attributes support])
fi
AM_CONDITIONAL(ENABLE_USER_XATTR, test x$enable_eua = xyes)

dnl Header checks
acl_headers=0

AC_CHECK_HEADERS([sys/acl.h],
[
acl_headers=1
], [], [])

AC_CHECK_HEADERS([acl/libacl.h],
[
acl_headers=1
], [], [])

if test x$acl_headers = x0;
then
  AC_MSG_ERROR([No suitable headers for ACL support have been found])
fi

dnl FreeBSD
AC_CHECK_FUNC([acl_get_perm_np], 
 [AC_DEFINE([HAVE_ACL_GET_PERM_NP], [], [acl_get_perm_np is available])]
[])

AC_SUBST(ACL_LIBS, [])
	
dnl Check ACL library
AC_CHECK_LIB(acl, acl_get_perm, 
 [
   AC_DEFINE([HAVE_ACL_GET_PERM], [], [acl_get_perm is available])
   AC_SUBST(ACL_LIBS, [-lacl])
 ],)

# This must be easier, right?
OLDCXXFLAGS="$CXXFLAGS"
OLDCPPFLAGS="$CPPFLAGS"
CXXFLAGS="$CXXFLAGS ${GTKMM_CFLAGS} ${NAUTILUS_EXTENSION_CFLAGS}"
CPPFLAGS="$CXXFLAGS ${GTKMM_CFLAGS} ${NAUTILUS_EXTENSION_CFLAGS}"
AC_CHECK_HEADERS([nautilus/nautilus-extension.h], [], [], [])
CXXFLAGS="$OLDCXXFLAGS"
CPPFLAGS="$OLDCPPFLAGS"

dnl Output files
AC_CONFIG_FILES([Makefile
		src/Makefile
		img/Makefile
		po/Makefile.in
		man/Makefile
		doc/Makefile
		doc/C/Makefile
		doc/C/figures/Makefile
		])
AC_OUTPUT

