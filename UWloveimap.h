#include <pthread.h>

#define PORT_OPEN	0	
#define PORT_CLOSE	1
#define PORT_NOTCHECKED	0
#define PORT_CHECKED	1

char username[16];
char password[16];
char imapsrv[32];
char target[32];
char ports[256];

int percent;
unsigned int progress;
unsigned int nbmiss;
unsigned int nbtotal;
unsigned int thread_number;
pthread_mutex_t portlistmutex;
pthread_mutex_t indexmutex;

struct structportlist
{
	int port;
	int checked;
	int status;
};

struct structportlist *portlist;

extern void construct_table(void);
extern void add_entry_table(int port);
extern void new_threads(void);
extern long getip(char *hostname);
extern int connect_host(char* host, int port);
extern void scan_port(void);
extern void imap_login(int sock);
extern int imap_select(int sock, int port);
