#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "UWloveimap.h"

#define IMAP_PORT 143

void imap_login(int sock)
{
	char buff[2048];
	read(sock, buff, sizeof(buff));
	memset(buff, 0, sizeof(buff));
	sprintf(buff, "1 LOGIN %s %s\r\n", username, password);
	write(sock, buff, strlen(buff));
	memset(buff, 0, sizeof(buff));
	read(sock, buff, sizeof(buff));

	if(strcasestr(buff, "1 NO LOGIN") != 0)
	{
		fprintf(stderr, "[!] login to imap server failed...\n");
		free(portlist);
		close(sock);
		exit(EXIT_FAILURE); 
	}
}

int imap_select(int sock, int port)
{
	char buff[2048];
	struct timespec ts;
	int i, nbread;

	memset(buff, 0, sizeof(buff));
	sprintf(buff, "1 SELECT \"{%s:%d}\"\r\n", target, port);
	write(sock, buff, strlen(buff));

	fcntl(sock, F_SETFL, O_NONBLOCK);
	ts.tv_sec = 1;
	ts.tv_nsec = 0;
	memset(buff, 0, sizeof(buff));

	for(i=0; i<10; i++)
	{
		nbread = read(sock, buff, sizeof(buff));
		if(nbread > 0) break;
		nanosleep(&ts, 0);		
	}

	if(nbread > 0)
	{
		if(strcasestr(buff, "connection refused") != 0) return PORT_CLOSE;
		else if(strcasestr(buff, "NO Error") != 0)
		{
			fprintf(stderr, "[!] Error: the targeted imapd dont accept the SELECT command\n");
			free(portlist);
			close(sock);
			exit(EXIT_FAILURE); 
		}
                else if(strcasestr(buff, "NO Mailbox does not exist") != 0)
                {
                        fprintf(stderr, "[!] Mailbox does not exit: imapd dont accept the SELECT command\n");
                        free(portlist);
                        close(sock);
                        exit(EXIT_FAILURE);
                }

		else return PORT_OPEN;	

	} else return PORT_OPEN; 
	
}

long getip(char *hostname)
{
        struct hostent *he;
        long ipaddr;

        if ((ipaddr = inet_addr(hostname)) < 0) {
                if ((he = gethostbyname(hostname)) == NULL) {
                        perror("gethostbyname()");
                        exit(0);
                }
                memcpy(&ipaddr, he->h_addr, he->h_length);
        }
        return ipaddr;
}

int connect_host(char* host, int port)
{
        struct sockaddr_in s_in;
        int sock;

        s_in.sin_family = AF_INET;
        s_in.sin_addr.s_addr = getip(host);
        s_in.sin_port = htons(port);

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
                fprintf(stderr, "[!] Could not create a socket\n");
		free(portlist);
                exit(EXIT_FAILURE);
        }

        if (connect(sock, (struct sockaddr *)&s_in, sizeof(s_in)) < 0) {
                fprintf(stderr, "[!] Connection to %s:%d failed: %s\n", host, port, strerror(errno));
		free(portlist);
                exit(EXIT_FAILURE);
        }

        return sock;
}

void scan_port(void)
{
	int index=0;
	int sock, portstatus;

	while(1)
	{
		if(portlist[index].checked == PORT_CHECKED)
		{
			pthread_mutex_lock(&indexmutex);
			index++;
			pthread_mutex_unlock(&indexmutex);
		} else {

			pthread_mutex_lock(&portlistmutex);
			if(portlist[index].port == 0)
			{
				nbmiss=0;
				pthread_mutex_unlock(&portlistmutex);
				break;
			}
		
			nbmiss--;
			portlist[index].checked = PORT_CHECKED;
			pthread_mutex_unlock(&portlistmutex);

			sock = connect_host(imapsrv, IMAP_PORT); 
			imap_login(sock);
			portstatus = imap_select(sock, portlist[index].port);
			
			pthread_mutex_lock(&portlistmutex);
			portlist[index].status = portstatus;
			pthread_mutex_unlock(&portlistmutex);

			if(portstatus == PORT_OPEN)
			{
				close(sock);
				break;
			}							

			pthread_mutex_lock(&indexmutex);
			index++;
			pthread_mutex_unlock(&indexmutex);

			if(nbmiss < (progress * (10 - percent)))
		        {
		                fprintf(stderr, " %d0%%..", percent);
        		        percent++;
		        }
		}
	}  

	pthread_exit(0);
}
