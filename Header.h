#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include </usr/include/semaphore.h>
#include "fifo.h"

#include <unistd.h>

#define BUFF_SIZE   5           /* total number of slots */
#define NP          3           /* total number of producers */
#define NC          3           /* total number of consumers */
#define NITERS      4           /* number of items produced/consumed */

typedef struct
{
    int buf[BUFF_SIZE];   /* shared var */
    int in;               /* buf[in%BUFF_SIZE] is the first empty slot */
    int out;              /* buf[out%BUFF_SIZE] is the first full slot */
    sem_t full;           /* keep track of the number of full spots */
    sem_t empty;          /* keep track of the number of empty spots */
    
    
    pthread_mutex_t mutex;          /* enforce mutual exclusion to shared data */
} sbuf_t;

sbuf_t shared;

