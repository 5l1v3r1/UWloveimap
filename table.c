#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "UWloveimap.h"

void construct_table(void)
{
	int x=0, y=0;
	char *retptr, *newptr, *tok_buff;

	portlist = malloc(sizeof(struct structportlist));

	if(strstr(ports, ","))
	{
		retptr=strtok_r(ports,",", &tok_buff);

		while(retptr)
		{
			if(strstr(retptr,"-"))
			{
				newptr=strtok(retptr, "-");
				add_entry_table(atoi(newptr));
				x = atoi(newptr);

				newptr=strtok(NULL, "-");

				if(x > atoi(newptr))
				{
					fprintf(stderr, "[!] strange port range...\n");
					free(portlist);
					exit(EXIT_FAILURE);
				}

				for(y=1; (x+y)<=atoi(newptr);y++)
				{
					add_entry_table(x+y);
				}

			} else {
				add_entry_table(atoi(retptr));
			}

			retptr=strtok_r(NULL, ",", &tok_buff);
		}
                
	} else if(strstr(ports, "-") && (retptr=strtok(ports, "-")))
	{
		x = atoi(retptr);
		add_entry_table(x);
		retptr=strtok(NULL, "-");

		if(x > atoi(retptr))
		{
			fprintf(stderr, "[!] strange port range...\n");
			free(portlist);
			exit(EXIT_FAILURE);
		}
        
		for(y=1; (x+y)<=atoi(retptr); y++)
		{
			add_entry_table(x+y);
		}
 
	} else {
		add_entry_table(atoi(ports));
	}

	add_entry_table(0);
}

void add_entry_table(int port)
{
	struct structportlist *ptr;

        if(port > 65535)
        {
                fprintf(stderr, "[!] strange port number...\n");
		free(portlist);
                exit(EXIT_FAILURE);
        }

	nbtotal++;

	if((ptr = realloc(portlist, (nbtotal)*sizeof(struct structportlist))) == NULL)
	{
		fprintf(stderr, "[!] realloc()\n");
		free(portlist);
		exit(EXIT_FAILURE);
	}	

	portlist = ptr;
	
	portlist[nbtotal-1].port = port;
	portlist[nbtotal-1].checked = PORT_NOTCHECKED;
	portlist[nbtotal-1].status = PORT_CLOSE;
}
