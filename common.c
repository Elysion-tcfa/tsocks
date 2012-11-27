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
void getipport(struct sockaddr* addr, void **ip, short *port) {
	if (addr->sa_family == AF_INET) {
		*ip = &((struct sockaddr_in *) addr) -> sin_addr;
		*port = ((struct sockaddr_in *) addr) -> sin_port;
	} else {
		*ip = &((struct sockaddr_in6 *) addr) -> sin6_addr;
		*port = ((struct sockaddr_in6 *) addr) -> sin6_port;
	}
}

int getsockaddrsize(int af) {
	if (af == AF_INET)
		return sizeof(struct sockaddr_in);
	else
		return sizeof(struct sockaddr_in6);
}

int getinaddrsize(int af) {
	if (af == AF_INET)
		return sizeof(struct in_addr);
	else
		return sizeof(struct in6_addr);
}

int match(int af, void *testip, void *ip, void *net) {
	if (af == AF_INET)
		return (((struct in_addr *) testip) -> s_addr & ((struct in_addr *) net) -> s_addr) == (((struct in_addr *) ip) -> s_addr & ((struct in_addr *) net) -> s_addr);
	else {
		int i = 0;
		for (; i < 16; i ++)
		if ((((struct in6_addr *) testip) -> s6_addr[i] & ((struct in6_addr *) net) -> s6_addr[i]) != (((struct in6_addr *) ip) -> s6_addr[i] & ((struct in6_addr *) net) -> s6_addr[i]))
				return 0;
		return 1;
	}
}

int check(int af, void *ip, void *net) {
	if (af == AF_INET)
		return (((struct in_addr *) ip) -> s_addr & ((struct in_addr *) net) -> s_addr)
			== ((struct in_addr *) ip) -> s_addr;
	else {
		int i = 0;
		for (; i < 16; i ++)
			if ((((struct in6_addr *) ip) -> s6_addr[i] & ((struct in6_addr *) net) -> s6_addr[i])
					!= ((struct in6_addr *) ip) -> s6_addr[i])
				return 0;
		return 1;
	}
}

int resolve_ip(int af, char *host, int showmsg, int allownames, void *host_addr) {

	int s;
	void *ip;
	short port;

	if (!allownames) {
		getipport(host_addr, &ip, &port);
		if ((s = inet_pton(af, host, ip)) != 1)
			return -1;
	}

	struct addrinfo hints, *result;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = af;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_TCP;

	if ((s = getaddrinfo(host, NULL, &hints, &result)) != 0)
		return -1;
	else {
		memcpy(host_addr, result -> ai_addr, getsockaddrsize(af));
		freeaddrinfo(result);
		if (showmsg) {
			char buf[60];
			getipport(host_addr, &ip, &port);
			inet_ntop(af, ip, buf, 60);
			printf("Connecting to %s...\n", buf);
		}
		return 0;
	}
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
void set_log_options(int level, char *filename, int timestamp) {

	loglevel = level;
	if (loglevel < MSGERR)
		loglevel = MSGNONE;

	if (filename) {
		strncpy(logfilename, filename, sizeof(logfilename));
		logfilename[sizeof(logfilename) - 1] = '\0';
	}

	logstamp = timestamp;
}

void show_msg(int level, char *fmt, ...) {
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