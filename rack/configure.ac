#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])

m4_define([ProjectName], [rackrtros])
m4_define([PrefixDefault], [/usr/local/]ProjectName)
m4_define([ConfigScript], [rackrtros-config])

AC_INIT([ProjectName],[m4_normalize(m4_include(config/version))],[],[ProjectName])

PROJNAME=ProjectName
AC_SUBST(PROJNAME)

CONFIG_RACKRTROS_VERSION_MAJOR=`expr $PACKAGE_VERSION : '\([0-9]*\)'`
CONFIG_RACKRTROS_VERSION_MINOR=`expr $PACKAGE_VERSION : '[0-9]*\.\([0-9]*\)'`
CONFIG_RACKRTROS_REVISION_LEVEL=`expr $PACKAGE_VERSION : '[0-9]*\.[0-9]*\.\([0-9]*\)'`
test "x$CONFIG_RACKRTROS_REVISION_LEVEL" = "x" && CONFIG_RACKRTROS_REVISION_LEVEL=0

AC_CONFIG_HEADERS([config/]ProjectName[_config_pre.h])
AC_CONFIG_SRCDIR([scripts/]ConfigScript[.in])
AC_PREFIX_DEFAULT(PrefixDefault)
AC_CONFIG_AUX_DIR([config/autoconf])
AC_CONFIG_MACRO_DIR([config/m4])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_PROG_INSTALL

CFLAGS=${CFLAGS:--s}


dnl ======================================================================
dnl checking cc
dnl ======================================================================

AC_ARG_WITH(CC,
    AS_HELP_STRING([--with-cc=compiler], [use specific C compiler]),
    [case "$withval" in
    "" | y | ye | yes | n | no)
  AC_MSG_ERROR([*** You must supply an argument to --with-cc.]) ;;
    esac
    CC="$withval"])
AC_PROG_CC
AC_PROG_CXX

AC_DEFINE_UNQUOTED(CONFIG_RACKRTROS_BUILD_STRING, "$build", [Build system alias])
RACKRTROS_BUILD_STRING="$build"
AC_DEFINE_UNQUOTED(CONFIG_RACKRTROS_HOST_STRING, "$host", [Host system alias])
RACKRTROS_HOST_STRING="$host"
AC_DEFINE_UNQUOTED(CONFIG_RACKRTROS_COMPILER, "`$CC -v 2>&1 | tail -n 1`" , [Compiler])

AM_INIT_AUTOMAKE([foreign no-exeext no-dist-gzip dist-bzip2])
AM_MAINTAINER_MODE
LT_INIT
AM_PROG_AS
AC_SUBST(LD)

AC_PATH_PROG(JAR, jar,)
AC_SUBST(JAR)

AC_C_INLINE
AC_HEADER_STDC

dnl ======================================================================
dnl  checking target arch
dnl ======================================================================

AC_MSG_CHECKING([for target architecture])
case "$host" in
  *86*-*)
    RACKRTROS_TARGET_ARCH=i386
    wanted_kernel_arch=CONFIG_X86
    ;;
  arm-*)
    RACKRTROS_TARGET_ARCH=arm
    wanted_kernel_arch=CONFIG_ARM
    ;;
  powerpc-*|ppc-*)
    RACKRTROS_TARGET_ARCH=ppc
    wanted_kernel_arch=CONFIG_PPC
    ;;
  *)
    AC_MSG_ERROR([ *** Sorry - Unsupported architecture])
    ;;
esac
AC_MSG_RESULT([${RACKRTROS_TARGET_ARCH}])
AC_SUBST(RACKRTROS_TARGET_ARCH)

dnl ======================================================================
dnl  Loading the RACKRTROS configuration.
dnl ======================================================================

RACKRTROS_KCONFIG_FILE=$srcdir/defconfig
kconfig_type=default

AC_ARG_WITH(kconfig-file,
    AS_HELP_STRING([--with-kconfig-file=<kconfig-file>], [KConfig parameter file]),
    [
        case "$withval" in
        "" | y | ye | yes | n | no)
          AC_MSG_ERROR([You must supply an argument to --with-kconfig-file.])
            ;;
        esac
        RACKRTROS_KCONFIG_FILE="$withval"
        kconfig_type=supplied
    ])

AC_MSG_CHECKING([for RACKRTROS Kconfig file])
if test \! -f $RACKRTROS_KCONFIG_FILE; then
    AC_MSG_ERROR([Kconfig-file $RACKRTROS_KCONFIG_FILE not found -- --with-kconfig-file sets Kconfig file])
fi

PATH=$PATH:. . $RACKRTROS_KCONFIG_FILE
AC_MSG_RESULT([$RACKRTROS_KCONFIG_FILE ($kconfig_type)])


dnl ======================================================================
dnl  XENOMAI userspace
dnl ======================================================================

AC_ARG_WITH(xeno-user-dir,
    AS_HELP_STRING([--with-xeno-user-dir=<Xenomai>], [Xenomai userspace installation path]),
    [case "${withval}" in
        "" | y | ye | yes | n | no)
            AC_MSG_ERROR([You must supply an argument to --with-xeno-user-dir.])
      ;;
    esac
    CONFIG_RACKRTROS_XENOMAIDIR="$withval"
    ])

XENO_USER_PREFIX=""
XENO_LIBRARIES=""

if test x"${CONFIG_RACKRTROS_OS_XENOMAI}" = x"y"; then
    if test x"$CONFIG_RACKRTROS_EXT_XENOMAIDIR" \!= "xy"; then
        CONFIG_RACKRTROS_XENOMAIDIR="${CONFIG_RACKRTROS_LIRE_INSTALLDIR}/xenomai/usr"
    fi
fi

if test "${CONFIG_RACKRTROS_XENOMAIDIR}" \!= "" ; then

dnl -----------------------------------------------------------------
dnl  try to find Xenomai config
dnl -----------------------------------------------------------------

AC_MSG_CHECKING([for Xenomai userspace installation])
XENO_USER_CONFIG="${CONFIG_RACKRTROS_XENOMAIDIR}/bin/xeno-config"
if test -x "${XENO_USER_CONFIG}"; then
  XENO_USER_DIR="`cd ${CONFIG_RACKRTROS_XENOMAIDIR} && pwd`"
  sed "s+\(^prefix=\)\(..*$\)+\1${XENO_USER_DIR}+" ${XENO_USER_CONFIG} > /tmp/${USER}.xeno_config
  cp /tmp/${USER}.xeno_config ${XENO_USER_CONFIG}
  rm /tmp/${USER}.xeno_config
  AC_MSG_RESULT([${XENO_USER_DIR}])
else
  AC_MSG_ERROR([no Xenomai found in ${CONFIG_RACKRTROS_XENOMAIDIR}])
fi

dnl -----------------------------------------------------------------
dnl  get Xenomai user prefix
dnl -----------------------------------------------------------------

AC_MSG_CHECKING([for Xenomai userspace PREFIX])
XENO_USER_PREFIX="`cd \`${XENO_USER_CONFIG} --prefix\` && pwd`"
AC_MSG_RESULT([${XENO_USER_PREFIX}])

dnl -----------------------------------------------------------------
dnl  checking Xenomai version
dnl -----------------------------------------------------------------

AC_MSG_CHECKING([for Xenomai version])
XENO_VERSION_PRE="`${XENO_USER_CONFIG} --version`"

case "$XENO_VERSION_PRE" in
  2.[[3-9]]*)
    XENO_VERSION=xeno-23x
    ;;
  *)
    AC_MSG_ERROR([*** Unsupported Xenomai version $XENO_VERSION_PRE in $XENO_USER_DIR])
  ;;
esac
AC_MSG_RESULT([${XENO_VERSION_PRE}])

dnl -----------------------------------------------------------------
dnl  get some more information about Xenomai
dnl -----------------------------------------------------------------

XENO_LIBRARIES="`${XENO_USER_CONFIG} --library-dir`"

fi

dnl -----------------------------------------------------------------
dnl export xenomai values
dnl -----------------------------------------------------------------

AC_SUBST(XENO_USER_PREFIX)
AC_SUBST(XENO_LIBRARIES)
AC_DEFINE(CONFIG_XENO_23X, 1, [We build for Xenomai 2.3 or newer])


dnl ======================================================================
dnl  directory / library checks
dnl ======================================================================

RACKRTROS_KMOD_CFLAGS=""
RACKRTROS_SUPPORTS=""

dnl ----- Xenomai --------------------------------------------------------
dnl
dnl  depends on nothing
dnl

AC_MSG_CHECKING([target os xenomai])
AC_MSG_RESULT([${CONFIG_RACKRTROS_OS_XENOMAI}])

XENOMAI_CPPFLAGS=""
XENOMAI_LDFLAGS=""
XENOMAI_LIBS=""

if test x"${CONFIG_RACKRTROS_OS_XENOMAI}" = x"y"; then

XENOMAI_CPPFLAGS="`${XENO_USER_CONFIG} --xeno-cflags`"
XENOMAI_LDFLAGS="`${XENO_USER_CONFIG} --xeno-ldflags`"
XENOMAI_LIBS="-lnative -lrtdm"
fi

AC_SUBST(XENOMAI_CPPFLAGS)
AC_SUBST(XENOMAI_LDFLAGS)
AC_SUBST(XENOMAI_LIBS)

AM_CONDITIONAL(CONFIG_RACKRTROS_OS_XENOMAI,
               [test x"${CONFIG_RACKRTROS_OS_XENOMAI}" = "xy"])


dnl ======================================================================
dnl  set flags / libs
dnl ======================================================================

RTROSRACK_GLOBAL_CPPFLAGS="-Wall -pipe -O2 -fstrict-aliasing"
RACKRTROS_GLOBAL_LDFLAGS=""

AC_SUBST(RACKRTROS_GLOBAL_CPPFLAGS)
AC_SUBST(RACKRTROS_GLOBAL_LDFLAGS)

if test x"${CONFIG_RACKRTROS_OS_XENOMAI}" = x"y"; then

RACKRTROS_CPPFLAGS="-D__IN_RACK__ -I\${top_srcdir} -I\${top_builddir} ${XENOMAI_CPPFLAGS} ${RACKRTROS_GLOBAL_CPPFLAGS}"
RACKRTROS_LDFLAGS="-L\$(top_builddir)/main/lib/.libs ${XENOMAI_LDFLAGS} ${RACKRTROS_GLOBAL_LDFLAGS}"
RACKRTROS_LIBS="-l${PROJNAME} ${XENOMAI_LIBS}"

else

RACKRTROS_CPPFLAGS="-D__IN_RACK__ -I\${top_srcdir} -I\${top_builddir} ${LINUX_CPPFLAGS} ${RACKRTROS_GLOBAL_CPPFLAGS}"
RACKRTROS_LDFLAGS="-L\$(top_builddir)/main/lib/.libs ${LINUX_LDFLAGS} ${RACKRTROS_GLOBAL_LDFLAGS}"
RACKRTROS_LIBS="-l${PROJNAME} ${LINUX_LIBS}"

fi

if test x"${CONFIG_RACKRTROS_PROXIES_MSG_SIZE_SCANDRIVE}" = x"y"; then
RACKRTROS_MSG_SIZE_CPPFLAGS="-D__MSG_SCANDRIVE__"
fi

if test x"${CONFIG_RACKRTROS_PROXIES_MSG_SIZE_VELODYNE}" = x"y"; then
RACKRTROS_MSG_SIZE_CPPFLAGS="-D__MSG_VELODYNE__"
fi

if test x"${CONFIG_RACKRTROS_PROXIES_MSG_SIZE_KINECT}" = x"y"; then
RACKRTROS_MSG_SIZE_CPPFLAGS="-D__MSG_KINECT__"
fi

RACKRTROS_CPPFLAGS="${RACKRTROS_MSG_SIZE_CPPFLAGS} ${RACKRTROS_CPPFLAGS}"

AC_SUBST(RACKRTROS_MSG_SIZE_CPPFLAGS)
AC_SUBST(RACKRTROS_CPPFLAGS)
AC_SUBST(RACKRTROS_LDFLAGS)
AC_SUBST(RACKRTROS_LIBS)


dnl ======================================================================
dnl  current project
dnl ======================================================================

RACKRTROS_SUPPORTS="${PROJNAME} ${RACKRTROS_SUPPORTS}"
AC_SUBST(RACKRTROS_SUPPORTS)


dnl ======================================================================
dnl             crosscompiling
dnl ======================================================================

AC_MSG_CHECKING([for CROSS_COMPILE])
if test "${CROSS_COMPILE}" = ""; then
    CROSS_COMPILE="`echo ${CC} | sed s/gcc//`"

    if test "${CROSS_COMPILE}gcc" \!= "${CC}"; then
        CROSS_COMPILE=""
        AC_MSG_RESULT([Cannot identify CROSS_COMPILE - assuming none - set manually if needed])
    else
        if test "${CROSS_COMPILE}" = ""; then
            AC_MSG_RESULT([none])
        else
            AC_MSG_RESULT([${CROSS_COMPILE}])
        fi
    fi
else
    AC_MSG_RESULT([${CROSS_COMPILE}])
fi
AC_SUBST(CROSS_COMPILE)

dnl ======================================================================
dnl             Installation directory for modules
dnl ======================================================================

AC_MSG_CHECKING([for module installation directory])

AC_ARG_WITH(module-dir,
    AS_HELP_STRING([--with-module-dir=<module-dir>], [Installation directory for modules]),
    [case "${withval}" in
    "" | y | ye | yes | n | no)
        AC_MSG_ERROR([You must supply an argument to --with-module-dir.]) ;;
    esac],
    with_module_dir='${exec_prefix}/lib/rack')
RACK_MODULE_DIR="$with_module_dir"
AC_MSG_RESULT([$RACKRTROS_MODULE_DIR])

AC_SUBST(RACKRTROS_MODULE_DIR)


dnl ======================================================================
dnl             generate output
dnl ======================================================================

AC_CONFIG_FILES([GNUmakefile \
    \
    config/GNUmakefile \
    config/kconfig/GNUmakefile \
    config/kconfig/lxdialog/GNUmakefile \
    \
    main/GNUmakefile \
    main/defines/GNUmakefile \
    main/lib/GNUmakefile \
    main/tools/GNUmakefile \
    main/common/GNUmakefile \
    main/xenomai/GNUmakefile \
    main/linux/GNUmakefile \
    main/tims/GNUmakefile \
    main/tims/xenomai/GNUmakefile \
    main/tims/xenomai_kmod/GNUmakefile \
    main/tims/linux/GNUmakefile \
    main/tims/router/GNUmakefile \
    \
    scripts/GNUmakefile \
    scripts/]ConfigScript[ \
    \
])

AC_OUTPUT

dnl ======================================================================
dnl generate ${PROJNAME}_config.h
dnl ======================================================================

AC_MSG_NOTICE([creating ]ProjectName[_config.h])
sed -e "s/ PACKAGE/ RACKRTROS_PACKAGE/" -e "s/ VERSION/ RACKRTROS_VERSION/" \
    config/${PROJNAME}_config_pre.h > ${PROJNAME}_config.h.new
if cmp -s ${PROJNAME}_config.h.new ${PROJNAME}_config.h; then
    rm ${PROJNAME}_config.h.new
    AC_MSG_NOTICE(ProjectName[_config.h is unchanged])
else
    mv ${PROJNAME}_config.h.new ${PROJNAME}_config.h
fi
