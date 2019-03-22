/*
 *   --=[UWloveimap]=--
 *
 * UWloveimap p.o.c of imap bounce scanner.
 * Use imapd for scanning hosts over inet or scanning
 * internal network trough an imap server shared between
 * inet and corporate network. Maybe usefull... just p.o.c
 * Greatz to XXX, khorben, coma, sead, qobaiashi and friends.
 * Coded by csk <casek@uberwall.org>
 * UberWall security team
 * Feedbacks welcomes
 * Glop glop eNjoy :p
 *
 *   --=[ Copyright ]=--
 *
 * Copyright (c) Uberwall 2003-2005
 * All rights reserved. All software redistribution and use
 * in source and binary forms, with or without modification, are
 * permitted provided that this copyright notice is retained.
 * All documentation belong to their respective authors.
 *
 *   --=[ Disclaimer ]=--
 *
 * We, authors gathered in the group "UberWall", can not be held
 * responsible for any use of the content available on its site,
 * "www.uberwall.org".
 *
 * Indeed, this group only aims to share knowledge and research
 * about computer security, and some elements presented may be
 * misused by malicious people.
 *
 * We warn again that anyone is responsible for his own behaviour,
 * and that unauthorized access to computer systems is illegal,
 * as well as any other activity assimilated to "cracking".
 *
 * We have absolutely no intention to break the law concerning any
 * resource or work detailed here.
 *
 *   --=[ Enjoy ]=--
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "UWloveimap.h"

void usage(char *soft)
{
        fprintf(stderr, "\nUsage:\n");
        fprintf(stderr, "%s -u login -p passwd -h imap_server -t target -P ports [option]\n\n", soft);
	fprintf(stderr, "Ports to scann can be for example: -P 22 or -P 21,23,25,80-110\n");
        fprintf(stderr, "Option: -n thread(s) Set number of scanning tHreads (default:5)\n\n");
	fprintf(stderr, "Using one thread is stealthy because of the low number of connections\n");
	fprintf(stderr, "needed to the imap server to perform the scan... but slower\n\n");
        exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	char cmdarg;
	int i;

	if(argc < 6) usage(argv[0]);

	nbtotal = 0;
	thread_number = 5;
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	memset(imapsrv, 0, sizeof(imapsrv));
	memset(target, 0, sizeof(target));
	memset(ports, 0, sizeof(ports));

	while((cmdarg = getopt(argc, argv, "u:p:h:t:P:n:")) != EOF)
	{
		switch(cmdarg)
		{
			case 'u':
				strncpy(username, optarg, sizeof(username));
				break;
			case 'p':
				strncpy(password, optarg, sizeof(password));
				break;
			case 'h':
				strncpy(imapsrv, optarg, sizeof(imapsrv));
				break;
			case 't':
				strncpy(target, optarg, sizeof(target));
				break;
			case 'P':
				strncpy(ports, optarg, sizeof(ports));
				break;
			case 'n':
				thread_number = atoi(optarg);
				break;				
			default:
				usage(argv[0]);
		}
	}

	if((username[0] == '\0') || (password[0] == '\0') ||
	   (imapsrv[0] == '\0') || (target[0] == '\0') ||
	   (ports[0] == '\0'))  usage(argv[0]);

        if(thread_number > 256)
        {
                fprintf(stderr, "[!] really.. more than 256 threads..\n");
                exit(EXIT_FAILURE);
        }

	construct_table();
	progress=nbtotal/10;
	nbmiss = nbtotal;
	percent = 1;
	fprintf(stderr, "[*] Starting scan\n[*]");
	while(nbmiss > 0) new_threads();
	fprintf(stderr, " 100%%... done\n");

	for(i=0; i<(int)nbtotal -1; i++)
	{
		if(portlist[i].status == PORT_CLOSE)
			fprintf(stdout, "[*] %s:%d\tstatus is CLOSED\n", target, portlist[i].port);
		if(portlist[i].status == PORT_OPEN)
			fprintf(stdout, "[*] %s:%d\tstatus is OPEN/FILTERED\n", target, portlist[i].port);
	}

	free(portlist);
	exit(EXIT_SUCCESS);	
}
