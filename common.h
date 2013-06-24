/* Common functions provided in common.c */

void getipport(struct sockaddr*, void **, short *);
int getsockaddrsize(int);
int getinaddrsize(int);
int check(int, void *, int);
int match(int, void *, void *, int);
void set_log_options(int, char *, int);
void show_msg(int level, char *, ...);
int resolve_ip(int, char *, int, int, void *);

#define MSGNONE   -1
#define MSGERR    0
#define MSGWARN   1
#define MSGNOTICE 2
#define MSGDEBUG  2
