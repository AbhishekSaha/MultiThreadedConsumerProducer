//
//  database.h
//  PA5: MultiThreaded
//
//  Created by Abhishek Saha on 4/7/14.
//  Copyright (c) 2014 Abhishek Saha. All rights reserved.
//

#ifndef PA5__MultiThreaded_database_h
#define PA5__MultiThreaded_database_h

#include "uthash.h"
#include <pthread.h>
#include <semaphore.h>
#include "fifo.h"

extern int ordernumber;

struct Packet{
    char * name;
    double wallet;
    fifo_t * reject;
    fifo_t * accept;
};


void databases(char * database);
void categories(char * cata);
typedef struct Packet Packet;
typedef Packet* PacketPtr;

PacketPtr PCreate(char * name, double wallet);


struct HashBucket {
    /* key */
    int ID;
    char * key;
    PacketPtr uzer;
    UT_hash_handle hh;         /* makes this structure hashable */
    
};

typedef struct HashBucket HashBucket;

int translate(char * string);

typedef struct
{
    struct fifo * queue;
    sem_t full;           /* keep track of the number of full spots */
    sem_t empty;          /* keep track of the number of empty spots */
    
    pthread_mutex_t mutex;          /* enforce mutual exclusion to shared data */
} sbuf_t;

void printout();
typedef struct Node{
    char * title;
    double cost;
    int ID;
    char * category;
    double balance;
} Node;



typedef Node* NodePtr;

NodePtr NodalCreate(char * title, double cost, int ID, char * category, double balance);

void* Producer(void *arg);

void prep(sbuf_t * rate);
void init_shared(size_t size);
struct OverSharedData *rinitialize();
void *alloc_shared(size_t size);
void alloc_rem(size_t size);

void frie(struct OverSharedData *);

#endif
