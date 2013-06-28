/*

    commmon.c    - Common routines for the tsocks package 

*/

#include "config.h"
#include <stdio.h>
#include <netdb.h>
#include "common.h"
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* Globals */
int loglevel = MSGERR;    /* The default logging level is to only log
                             error messages */
char logfilename[256];    /* Name of file to which log messages should
                             be redirected */
FILE *logfile = NULL;     /* File to which messages should be logged */
int logstamp = 0;         /* Timestamp (and pid stamp) messages */

/* Misc IPv4/v6 compatibility functions */
void __attribute__ ((visibility ("hidden"))) getipport(struct sockaddr* addr, void **ip, short *port) {
#ifdef ENABLE_IPV6
	if (addr->sa_family == AF_INET) {
#endif
		*ip = &((struct sockaddr_in *) addr) -> sin_addr;
		*port = ((struct sockaddr_in *) addr) -> sin_port;
#ifdef ENABLE_IPV6
	}
	else {
		*ip = &((struct sockaddr_in6 *) addr) -> sin6_addr;
		*port = ((struct sockaddr_in6 *) addr) -> sin6_port;
	}
#endif
}

int __attribute__ ((visibility ("hidden"))) getsockaddrsize(int af) {
#ifdef ENABLE_IPV6
	if (af == AF_INET)
#endif
		return sizeof(struct sockaddr_in);
#ifdef ENABLE_IPV6
	else
		return sizeof(struct sockaddr_in6);
#endif
}

int __attribute__ ((visibility ("hidden"))) getinaddrsize(int af) {
#ifdef ENABLE_IPV6
	if (af == AF_INET)
#endif
		return sizeof(struct in_addr);
#ifdef ENABLE_IPV6
	else
		return sizeof(struct in6_addr);
#endif
}

#ifndef HAVE_INET_PTON
int __attribute__ ((visibility ("hidden"))) inet_pton(int af, const char *src, void *dst) {
#ifdef HAVE_INET_ATON
	int ret = inet_aton(src, dst);
	return ret != 0;
#else
	in_addr_t ret = inet_addr(src);
	if (ret = (unsigned) -1) return 0;
	else {
		* (in_addr *) dst = ret;
		return 1;
	}
#endif
}
#endif

#ifndef HAVE_INET_NTOP
const char *__attribute__ ((visibility ("hidden"))) inet_ntop(int af, const void *src, char *dst, socklen_t size) {
	char *res = inet_ntoa(* (struct in_addr *) src);
	if (strlen(res) > size) {
		errno = ENOSPC;
		return NULL;
	}
	strcpy(dst, res);
	return dst;
}
#endif

int __attribute__ ((visibility ("hidden"))) match(int af, void *testip, void *ip, int net) {
	int i = 0;
	unsigned char *testaddr, *addr;
#ifdef ENABLE_IPV6
	if (af == AF_INET) {
#endif
		testaddr = (unsigned char *) & ((struct in_addr *) testip) -> s_addr;
		addr = (unsigned char *) & ((struct in_addr *) ip) -> s_addr;
		for (; i < net / 8; i++)
			if (testaddr[i] != addr[i])
				return 0;
		if (net % 8)
			return (testaddr[i] & (-1) << (8 - net % 8)) == (addr[i] & (-1) << (8 - net % 8));
		else
			return 1;
#ifdef ENABLE_IPV6
	} else {
		testaddr = ((struct in6_addr *) testip) -> s6_addr;
		addr = ((struct in6_addr *) ip) -> s6_addr;
		for (; i < net / 8; i++)
			if (testaddr[i] != addr[i])
				return 0;
		if (net % 8)
			return (testaddr[i] & (-1) << (8 - net % 8)) == (addr[i] & (-1) << (8 - net % 8));
		else
			return 1;
	}
#endif
}

int __attribute__ ((visibility ("hidden"))) check(int af, void *ip, int net) {
	int i = net / 8 + 1;
	unsigned char *addr;
#ifdef ENABLE_IPV6
	if (af == AF_INET) {
#endif
		addr = (unsigned char *) & ((struct in_addr *) ip) -> s_addr;
		for (; i < 4; i++)
			if (addr[i] != 0)
				return 0;
		if (net < 32)
			return (addr[net / 8] & (-1) << (8 - net % 8)) == addr[net / 8];
		else
			return 1;
#ifdef ENABLE_IPV6
	} else {
		addr = ((struct in6_addr *) ip) -> s6_addr;
		for (; i < 16; i++)
			if (addr[i] != 0)
				return 0;
		if (net < 128)
			return (addr[net / 8] & (-1) << (8 - net % 8)) == addr[net / 8];
		else
			return 1;
	}
#endif
}

int __attribute__ ((visibility ("hidden"))) resolve_ip(int af, char *host, int showmsg, int allownames, void *host_addr) {

	int s;
	void *ip;
	short port;

	if (!allownames) {
		getipport(host_addr, &ip, &port);
		if ((s = inet_pton(af, host, ip)) != 1)
			return -1;
	}

#ifdef HAVE_GETADDRINFO
	struct addrinfo hints, *result;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = af;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_TCP;

	if ((s = getaddrinfo(host, NULL, &hints, &result)) != 0)
		return -1;
	memcpy(host_addr, result -> ai_addr, result -> ai_addrlen);
	freeaddrinfo(result);
#else
	struct hostent *new;

	if ((new = gethostbyname(host)) == NULL || new -> h_addrtype != af)
		return -1;
	((struct sockaddr_in *) host_addr) -> sin_family = af;
	memcpy(&((struct sockaddr_in *) host_addr) -> sin_addr.s_addr, * new -> h_addr_list, new -> h_length);
#endif

	if (showmsg) {
		char buf[60];

		getipport(host_addr, &ip, &port);
		inet_ntop(af, ip, buf, 60);
		printf("Connecting to %s...\n", buf);
	}
	return 0;
}

/* Set logging options, the options are as follows:             */
/*  level - This sets the logging threshold, messages with      */
/*          a higher level (i.e lower importance) will not be   */
/*          output. For example, if the threshold is set to     */
/*          MSGWARN a call to log a message of level MSGDEBUG   */
/*          would be ignored. This can be set to -1 to disable  */
/*          messages entirely                                   */
/*  filename - This is a filename to which the messages should  */
/*             be logged instead of to standard error           */
/*  timestamp - This indicates that messages should be prefixed */
/*              with timestamps (and the process id)            */
void __attribute__ ((visibility ("hidden"))) set_log_options(int level, char *filename, int timestamp) {

	loglevel = level;
	if (loglevel < MSGERR)
		loglevel = MSGNONE;

	if (filename) {
		strncpy(logfilename, filename, sizeof(logfilename));
		logfilename[sizeof(logfilename) - 1] = '\0';
	}

	logstamp = timestamp;
}

void __attribute__ ((visibility ("hidden"))) show_msg(int level, char *fmt, ...) {
	va_list ap;
	int saveerr;
	extern char *progname;
	char timestring[20];
	time_t timestamp;

	if ((loglevel == MSGNONE) || (level > loglevel))
		return;

	if (!logfile) {
		if (logfilename[0]) {
			logfile = fopen(logfilename, "a");
			if (logfile == NULL) {
				logfile = stderr;
				show_msg(MSGERR, "Could not open log file, %s, %s\n",
							logfilename, strerror(errno));
			}
		} else
			logfile = stderr;
	}

	if (logstamp) {
		timestamp = time(NULL);
		strftime(timestring, sizeof(timestring), "%H:%M:%S",
					localtime(&timestamp));
		fprintf(logfile, "%s ", timestring);
	}

	fputs(progname, logfile);

	if (logstamp) {
		fprintf(logfile, "(%d)", getpid());
	}

	fputs(": ", logfile);

	va_start(ap, fmt);

	/* Save errno */
	saveerr = errno;

	vfprintf(logfile, fmt, ap);

	fflush(logfile);

	errno = saveerr;

	va_end(ap);
}
