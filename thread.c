#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "UWloveimap.h"

struct th_id
{
        int flag;
        pthread_t th;
};

void new_threads(void)
{
        pthread_t id;
        void *th_ret;
        int i;
        struct th_id th_id[256];

        memset(th_id, 0, sizeof(th_id));

        pthread_mutex_init(&portlistmutex, NULL);
	pthread_mutex_init(&indexmutex, NULL);
        
        if(nbmiss < thread_number) thread_number = nbmiss;

        for(i=0; i<(int)thread_number; i++)
	{
                if(pthread_create(&id, NULL, (void *)scan_port, NULL) != 0)
                {
                        fprintf(stderr, "[!] not enought ressources for thread creation\n");
                        exit(EXIT_FAILURE);
                } 
        
                th_id[i].flag = 1;
                th_id[i].th = id;		
	}

        for(i=0; i<(int)thread_number; i++)
        {
                if(th_id[i].flag == 1)  
                {
                        if(pthread_join(th_id[i].th, &th_ret) != 0)
                        fprintf(stderr, "[!] thread %d has some pb to be joined\n", (int)th_id[i].th);
                }
        }
}
