dnl Process this file with autoconf to produce a configure script.
AC_INIT
AC_CONFIG_SRCDIR([saveme.c])
AC_CONFIG_HEADER(config.h)

dnl Our default prefix is /usr/ since most people will be using tsocks
dnl on Linux systems and that /usr/local/ stuff annoys them
AC_PREFIX_DEFAULT(/usr)

dnl autoheader templates
AH_TEMPLATE([ALLOW_MSG_OUTPUT],
			[Allow tsocks to generate messages to stderr when errors are
			 encountered, this is really important and should only be disabled if
			 you're REALLY sure. It can also be turned off at run time, see the man
			 page for details])
AH_TEMPLATE([ALLOW_ENV_CONFIG],
			[Allow TSOCKS_CONF_FILE in environment to specify config file
			 location])
AH_TEMPLATE([USE_GNU_SOURCE],
			[Use _GNU_SOURCE to define RTLD_NEXT, mostly for RH7 systems])
AH_TEMPLATE([USE_OLD_DLSYM],
			[dlopen() the old libc to get connect() instead of RTLD_NEXT, 
			 hopefully shouldn't be needed])
AH_TEMPLATE([ENABLE_IPV6],
			[Enable ipv6 interception in tsocks])
AH_TEMPLATE([LIBCONNECT],
			[Path to library containing connect(), needed if USE_OLD_DLSYM is enabled])
AH_TEMPLATE([LIBC],
			[Path to libc, needed if USE_OLD_DLSYM is enabled])
AH_TEMPLATE([USE_SOCKS_DNS],
			[Configure the system resolver to use TCP queries on startup, this
			 allows socksified DNS])
AH_TEMPLATE([CONNECT_SIGNATURE],
			[Prototype and function header for connect function])
AH_TEMPLATE([CONNECT_SOCKARG],
			[The type of socket structure pointer to use to call the 
			 real connect])
AH_TEMPLATE([SELECT_SIGNATURE],
			[Prototype and function header for select function])
AH_TEMPLATE([POLL_SIGNATURE],
			[Prototype and function header for poll function])
AH_TEMPLATE([CLOSE_SIGNATURE],
			[Prototype and function header for close function])
AH_TEMPLATE([SEND_SIGNATURE],
			[Prototype and function header for send function])
AH_TEMPLATE([SENDTO_SIGNATURE],
			[Prototype and function header for sendto function])
AH_TEMPLATE([WRITE_SIGNATURE],
			[Prototype and function header for write function])
AH_TEMPLATE([RECV_SIGNATURE],
			[Prototype and function header for recv function])
AH_TEMPLATE([RECVFROM_SIGNATURE],
			[Prototype and function header for recvfrom function])
AH_TEMPLATE([READ_SIGNATURE],
			[Prototype and function header for read function])
AH_TEMPLATE([GETPEERNAME_SIGNATURE],
			[Prototype and function header for getpeername function])
AH_TEMPLATE([HAVE_INET_ADDR],
			[Work out which function we have for conversion from string IPs to 
			 numerical ones])
AH_TEMPLATE([HAVE_INET_ATON],
			[Work out which function we have for conversion from string IPs to 
			 numerical ones])
AH_TEMPLATE([HAVE_INET_PTON],
			[Work out which function we have for conversion from string IPs to 
			 numerical ones])
AH_TEMPLATE([HAVE_INET_NTOA],
			[Work out which function we have for conversion from numerical IPs to 
			 string ones])
AH_TEMPLATE([HAVE_INET_NTOP],
			[Work out which function we have for conversion from numerical IPs to 
			 string ones])
AH_TEMPLATE([DEFINE_STRSEP],
			[We use strsep which isn't on all machines, but we provide our own
			 definition of it for those which don't have it, this causes us to define
			 our version])
AC_DEFINE([HOSTNAMES], 0,
		  [Allow the use of DNS names in the socks configuration file for socks
		   servers. This doesn't work if socksified DNS is enabled for obvious
		   reasons, it also introduces overhead, but people seem to want it])
AH_TEMPLATE([HAVE_GETHOSTBYNAME],
			[We need the gethostbyname() function to do dns lookups in tsocks or 
			 in inspectsocks])
AH_TEMPLATE([HAVE_GETADDRINFO],
			[We need the getaddrinfo() function to do dns lookups in tsocks or 
			 in inspectsocks])
AH_TEMPLATE([CONF_FILE],
			[Location of configuration file (typically /etc/tsocks.conf)])

dnl if libdir hasn't been set by the user default it to /lib since
dnl tsocks needs to be on the root partition if put in the
dnl /etc/ld.so.preload file
test "$libdir" = "\${exec_prefix}/lib" && libdir="/lib"

dnl Arguments we allow
AC_ARG_ENABLE(socksdns,
[  --enable-socksdns	      force dns lookups to use tcp ])
AC_ARG_ENABLE(debug,
[  --disable-debug         disable ALL error messages from tsocks ])
AC_ARG_ENABLE(oldmethod,
[  --enable-oldmethod	   use the old method to override connect ])
AC_ARG_ENABLE(hostnames,
[  --disable-hostnames	   disable hostname lookups for socks servers ])
AC_ARG_ENABLE(envconf,
[  --disable-envconf       do not allow TSOCKS_CONF_FILE to specify configuration file ])
AC_ARG_WITH(conf,
[  --with-conf=<file>      location of configuration file (/etc/tsocks.conf default)],[
if test "${withval}" = "yes" ; then
  AC_MSG_ERROR([--with-conf requires the location of the configuration file as an argument])
else
  AC_DEFINE_UNQUOTED(CONF_FILE, "${withval}")
fi
], [
  AC_DEFINE_UNQUOTED(CONF_FILE, "/etc/tsocks.conf")
])


dnl -----------------------------------
dnl Get hostname and other information.
dnl -----------------------------------
AC_CANONICAL_HOST

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

dnl Check if the C compiler accepts -Wall
AC_MSG_CHECKING([if the C compiler accepts -Wall])
OLDCFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],[AC_MSG_RESULT(yes)],[
   CFLAGS="$OLDCFLAGS"
   AC_MSG_RESULT(no)])

dnl Checks for standard header files.
AC_HEADER_STDC

dnl Check for the dynamic loader function header
AC_CHECK_HEADER(dlfcn.h,,AC_MSG_ERROR([dlfcn.h not found]))

dnl Check for the socket header
AC_CHECK_HEADER(sys/socket.h,,AC_MSG_ERROR([sys/socket.h not found]))

dnl Check for the arpa/inet.h header (inet_ntoa and inet_addr)
AC_CHECK_HEADER(arpa/inet.h,,AC_MSG_ERROR([arpa/inet.h not found]))

dnl Check for the fcntl header
AC_CHECK_HEADER(fcntl.h,,AC_MSG_ERROR([fcntl.h not found]))

dnl Check for the poll header
AC_CHECK_HEADER(sys/poll.h,,AC_MSG_ERROR([sys/poll.h not found]))

dnl Other headers we're interested in
AC_CHECK_HEADERS(unistd.h)

dnl Checks for library functions.
AC_CHECK_FUNCS(strcspn strdup strerror strspn strtol,,[
	       AC_MSG_ERROR([Required function not found])])

dnl First find the library that contains connect() (obviously
dnl the most important library for us). Once we've found it
dnl we chuck it on the end of LIBS, that lib may end up there
dnl more than once (since we do our search with an empty libs
dnl list) but that isn't a problem
OLDLIBS="${LIBS}"
LIBS=
CONNECTLIB=
for LIB in c socket; do
  AC_CHECK_LIB("${LIB}",connect,[
    CONNECTLIB="${LIB}"
    break
  ],)
done
LIBS="${OLDLIBS} -l${CONNECTLIB}"
if test "${CONNECTLIB}" = ""; then
  AC_MSG_ERROR([Could not find library containing connect()])
fi

dnl Check for socket
AC_CHECK_FUNC(socket,, [
  AC_CHECK_LIB(socket, socket,,AC_MSG_ERROR([socket function not found]))])

dnl Check for a function to convert an ascii ip address
dnl to a sin_addr.
AC_CHECK_FUNC(inet_pton, AC_DEFINE(HAVE_INET_PTON), [
  AC_CHECK_FUNC(inet_aton, AC_DEFINE(HAVE_INET_ATON), [
    AC_CHECK_FUNC(inet_addr, AC_DEFINE(HAVE_INET_ADDR), [
      AC_CHECK_LIB(nsl, inet_addr, [ AC_DEFINE(HAVE_INET_ADDR)
                                     LIBS="${LIBS} -lnsl" ], [
  		AC_MSG_ERROR([Neither inet_pton, inet_aton or inet_addr present])])])])])

dnl Check for a function to convert a sin_addr to an
dnl ascii ip address.
AC_CHECK_FUNC(inet_ntop, AC_DEFINE(HAVE_INET_NTOP), [
  AC_CHECK_FUNC(inet_ntoa, AC_DEFINE(HAVE_INET_NTOA), [
  		AC_MSG_ERROR([Neither inet_ntop or inet_ntoa present])])])

dnl Look for getaddrinfo/gethostbyname (needed by tsocks and inspectsocks)
AC_CHECK_FUNC(getaddrinfo, AC_DEFINE(HAVE_GETADDRINFO), [
  AC_CHECK_FUNC(gethostbyname, AC_DEFINE(HAVE_GETHOSTBYNAME), [
    AC_CHECK_LIB(xnet, gethostbyname, AC_DEFINE(HAVE_GETHOSTBYNAME), [
  	  AC_MSG_ERROR([gethostbyname not found, name lookups in
  		      tsocks and inspectsocks disabled])])])])

dnl Check if constant AF_INET6 is present
if test "x$ac_cv_func_inet_pton" = "xyes" && test "x$ac_cv_func_getaddrinfo" = "xyes"; then
AC_EGREP_CPP(yes,
[
 #include <sys/socket.h>
 #ifdef AF_INET6
   yes
 #endif
], [AC_DEFINE(ENABLE_IPV6)], [AC_MSG_RESULT(ipv6 is disabled)])
else
AC_MSG_RESULT(ipv6 is disabled)
fi

dnl The simple programs (saveme and inspectsocks) have no further
dnl requirements, so save the libs needed here and use them in the
dnl Makefile
SIMPLELIBS=${LIBS}
LIBS=

dnl Checks for libraries.
dnl Replace `main' with a function in -ldl:
AC_CHECK_LIB(dl, dlsym,,AC_MSG_ERROR([libdl is required]))

dnl If we're using gcc here define _GNU_SOURCE
AC_MSG_CHECKING([for RTLD_NEXT from dlfcn.h])
AC_EGREP_CPP(yes,
[
 #include <dlfcn.h>
 #ifdef RTLD_NEXT
	yes
 #endif
], [
  AC_MSG_RESULT(yes)
], [
  AC_MSG_RESULT(no)
  AC_MSG_CHECKING([for RTLD_NEXT from dlfcn.h with _GNU_SOURCE])
  AC_EGREP_CPP(yes,
  [
   #define _GNU_SOURCE
   #include <dlfcn.h>
   #ifdef RTLD_NEXT
	yes
   #endif
  ], [
    AC_MSG_RESULT(yes)
    AC_DEFINE(USE_GNU_SOURCE)
  ], [
    AC_MSG_RESULT(no)
    AC_DEFINE(USE_OLD_DLSYM)
    oldmethod="yes"
  ])
])

if test "${enable_socksdns}" = "yes"; then
  AC_DEFINE(USE_SOCKS_DNS)
fi

if test "x${enable_envconf}" = "x"; then
  AC_DEFINE(ALLOW_ENV_CONFIG)
fi

if test "${enable_oldmethod}" = "yes"; then
  AC_DEFINE(USE_OLD_DLSYM)
  oldmethod="yes"
fi

if test "x${enable_debug}" = "x"; then
  AC_DEFINE(ALLOW_MSG_OUTPUT)
fi

if test "x${enable_hostnames}" = "x"; then
  AC_DEFINE(HOSTNAMES)
fi

if test "${enable_socksdns}" = "yes" -a \
        "x${enable_hostnames}" = "x" ; then
  AC_MSG_ERROR([--enable-socksdns is not valid without --disable-hostnames])
fi

dnl If we have to use the old method of overriding connect (i.e no
dnl RTLD_NEXT) we need to know the location of the library that
dnl contains connect(), select(), poll() and close()
if test "${oldmethod}" = "yes"; then
  dnl We need to find the path to the library, to do
  dnl this we use find on the usual suspects, i.e /lib and
  dnl /usr/lib

  dnl Check that find is available, it should be somehere
  dnl in the path
  AC_CHECK_PROG(FIND, find, find)
  if test "${FIND}" = ""; then
    AC_MSG_ERROR([find not found in path])
  fi

  dnl Find tail, it should always be somewhere in the path
  dnl but for safety's sake
  AC_CHECK_PROG(TAIL, tail, tail)
  if test "${TAIL}" = ""; then
    AC_MSG_ERROR([tail not found in path])
  fi

  dnl Now find the library we need
  AC_MSG_CHECKING([location of lib${CONNECTLIB}.so])
  LIBCONNECT=
  for DIR in '/lib' '/usr/lib'; do
    if test "${LIBCONNECT}" = ""; then
      LIBCONNECT=`$FIND $DIR -name "lib${CONNECTLIB}.so.?" 2>/dev/null | $TAIL -1`
    fi
  done

  AC_DEFINE_UNQUOTED(LIBCONNECT, "${LIBCONNECT}")
  if test "${LIBCONNECT}" = ""; then
     AC_MSG_ERROR([not found!])
  fi

  AC_MSG_RESULT($LIBCONNECT)

  dnl close() should be in libc, find it
  AC_MSG_CHECKING([location of libc.so])
  LIBC=
  for DIR in '/lib' '/usr/lib'; do
    if test "${LIBC}" = ""; then
      LIBC=`$FIND $DIR -name "libc.so.?" 2>/dev/null | $TAIL -1`
    fi
  done

  AC_DEFINE_UNQUOTED(LIBC, "${LIBC}")
  if test "${LIBC}" = ""; then
     AC_MSG_ERROR([not found!])
  fi

  AC_MSG_RESULT($LIBC)
fi

dnl Find the correct select prototype on this machine
AC_MSG_CHECKING(for correct select prototype)
PROTO=
for testproto in 'int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout'
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <sys/time.h>
      #include <sys/types.h>
      #include <unistd.h>
      int select($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([select(${PROTO})])
AC_DEFINE_UNQUOTED(SELECT_SIGNATURE, [${PROTO}])

dnl Find the correct connect prototype on this machine
AC_MSG_CHECKING(for correct connect prototype)
PROTO=
PROTO1='int __fd, const struct sockaddr * __addr, int __len'
PROTO2='int __fd, const struct sockaddr_in * __addr, socklen_t __len'
PROTO3='int __fd, struct sockaddr * __addr, int __len'
PROTO4='int __fd, const struct sockaddr * __addr, socklen_t __len'
for testproto in "${PROTO1}" \
                 "${PROTO2}" \
                 "${PROTO3}" \
                 "${PROTO4}"
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <sys/socket.h>
      int connect($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([connect(${PROTO})])
AC_DEFINE_UNQUOTED(CONNECT_SIGNATURE, [${PROTO}])

dnl Pick which of the sockaddr type arguments we need for
dnl connect(), we need to cast one of ours to it later
SOCKETARG="struct sockaddr *"
case "${PROTO}" in
   *sockaddr_in*)
      SOCKETARG="struct sockaddr_in *"
      ;;
esac
AC_DEFINE_UNQUOTED(CONNECT_SOCKARG, [${SOCKETARG}])

dnl Find the correct close prototype on this machine
AC_MSG_CHECKING(for correct close prototype)
PROTO=
PROTO1='int fd'
for testproto in "${PROTO1}"
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <stdlib.h>
      int close($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([close(${PROTO})])
AC_DEFINE_UNQUOTED(CLOSE_SIGNATURE, [${PROTO}])

dnl Find the correct poll prototype on this machine
AC_MSG_CHECKING(for correct poll prototype)
PROTO=
for testproto in 'struct pollfd *ufds, unsigned long nfds, int timeout'
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <sys/poll.h>
      int poll($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([poll(${PROTO})])
AC_DEFINE_UNQUOTED(POLL_SIGNATURE, [${PROTO}])

dnl Find the correct send prototype on this machine
AC_MSG_CHECKING(for correct send prototype)
PROTO=
for testproto in 'int socket, const void *buffer, size_t length, int flags'
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <sys/socket.h>
      ssize_t send($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([send(${PROTO})])
AC_DEFINE_UNQUOTED(SEND_SIGNATURE, [${PROTO}])

dnl Find the correct sendto prototype on this machine
AC_MSG_CHECKING(for correct sendto prototype)
PROTO=
PROTO1='int socket, const void *message, size_t length, int flags, const struct sockaddr * dest_addr, int dest_len'
PROTO2='int socket, const void *message, size_t length, int flags, const struct sockaddr_in * dest_addr, socklen_t dest_len'
PROTO3='int socket, const void *message, size_t length, int flags, struct sockaddr * dest_addr, int dest_len'
PROTO4='int socket, const void *message, size_t length, int flags, const struct sockaddr * dest_addr, socklen_t dest_len'
for testproto in "${PROTO1}" \
                 "${PROTO2}" \
                 "${PROTO3}" \
                 "${PROTO4}"
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <sys/socket.h>
      ssize_t sendto($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([sendto(${PROTO})])
AC_DEFINE_UNQUOTED(SENDTO_SIGNATURE, [${PROTO}])

dnl Find the correct write prototype on this machine
AC_MSG_CHECKING(for correct write prototype)
PROTO=
for testproto in 'int fildes, const void *buf, size_t nbyte'
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <unistd.h>
      ssize_t write($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([write(${PROTO})])
AC_DEFINE_UNQUOTED(WRITE_SIGNATURE, [${PROTO}])

dnl Find the correct recv prototype on this machine
AC_MSG_CHECKING(for correct recv prototype)
PROTO=
for testproto in 'int socket, void *buffer, size_t length, int flags'
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <sys/socket.h>
      ssize_t recv($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([recv(${PROTO})])
AC_DEFINE_UNQUOTED(RECV_SIGNATURE, [${PROTO}])

dnl Find the correct recvfrom prototype on this machine
AC_MSG_CHECKING(for correct recvfrom prototype)
PROTO=
PROTO1='int socket, void *buffer, size_t length, int flags, struct sockaddr * address, int *address_len'
PROTO2='int socket, void *buffer, size_t length, int flags, struct sockaddr_in * address, socklen_t *address_len'
PROTO3='int socket, void *buffer, size_t length, int flags, struct sockaddr * address, int *address_len'
PROTO4='int socket, void *buffer, size_t length, int flags, struct sockaddr * address, socklen_t *address_len'
for testproto in "${PROTO1}" \
                 "${PROTO2}" \
                 "${PROTO3}" \
                 "${PROTO4}"
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <sys/socket.h>
      ssize_t recvfrom($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([recvfrom(${PROTO})])
AC_DEFINE_UNQUOTED(RECVFROM_SIGNATURE, [${PROTO}])

dnl Find the correct write prototype on this machine
AC_MSG_CHECKING(for correct read prototype)
PROTO=
for testproto in 'int fildes, void *buf, size_t nbyte'
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <unistd.h>
      ssize_t read($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([read(${PROTO})])
AC_DEFINE_UNQUOTED(READ_SIGNATURE, [${PROTO}])

dnl Find the correct getpeername prototype on this machine
AC_MSG_CHECKING(for correct getpeername prototype)
PROTO=
PROTO1='int __fd, const struct sockaddr * __name, int *__namelen'
PROTO2='int __fd, const struct sockaddr_in * __name, socklen_t *__namelen'
PROTO3='int __fd, struct sockaddr * __name, socklen_t *__namelen'
PROTO4='int __fd, const struct sockaddr * __name, socklen_t *__namelen'
for testproto in "${PROTO1}" \
                 "${PROTO2}" \
                 "${PROTO3}" \
                 "${PROTO4}"
do
  if test "${PROTO}" = ""; then
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      #include <sys/socket.h>
      int getpeername($testproto);
    ]], [[]])],[PROTO="$testproto";],[])
  fi
done
if test "${PROTO}" = ""; then
  AC_MSG_ERROR([no match found!])
fi
AC_MSG_RESULT([getpeername(${PROTO})])
AC_DEFINE_UNQUOTED(GETPEERNAME_SIGNATURE, [${PROTO}])

dnl Output the special librarys (libdl etc needed for tsocks)
SPECIALLIBS=${LIBS}
AC_SUBST(SPECIALLIBS)
LIBS=${SIMPLELIBS}

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
