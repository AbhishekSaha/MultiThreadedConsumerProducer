//
//  main.c
//  PA5: MultiThreaded
//
//  Created by Abhishek Saha on 4/7/14.
//  Copyright (c) 2014 Abhishek Saha. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "uthash.h"
#include "database.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

HashBucket * users = NULL;
HashBucket * categores = NULL;
int consumerthreads;
char * bookorders;
int flags = -1;
double revenue;
pthread_mutex_t grill;

sbuf_t * consumers;

#define	FALSE	0
#define TRUE	1
#define SBUFSIZE        16

char * dtabase;

struct SharedData {
    int da;
    int	        isopen;
    int		refcount;	// reference count:  number of threads using this object
    unsigned int    front;		// subscript of front of queue
    unsigned int    count;		// number of chars in queue
    unsigned int    bufsize;
    pthread_cond_t buffer_full;
    pthread_cond_t buffer_empty;
    pthread_mutex_t mtex;
    fifo_t* queue;
    sem_t		empty_count;
    
    sem_t		full_count;
    sem_t		use_queue;	// mutual exclusion
};

struct OverSharedData{
    struct SharedData ** rep;
    int rop;
};

pthread_cond_t finis;

void
initialize( struct SharedData * sptr, int rc )		// Looks like a ctor
{
    fifo_t * bill = (fifo_t*)malloc(sizeof(fifo_t));
    bill = fifo_new();
    sptr->queue = bill;
	sptr->isopen = TRUE;
	sptr->refcount = rc;
	sptr->front = 0;
	sptr->count = 0;
	sptr->bufsize = SBUFSIZE;
	sem_init( &sptr->empty_count, 0, 10 );
    pthread_mutex_init(&sptr->mtex, 0);
    pthread_cond_init(&sptr->buffer_empty, 0);
    pthread_cond_init(&sptr->buffer_full, 0);
	sem_init( &sptr->full_count, 0, 0 );
	sem_init( &sptr->use_queue, 0, 1 );
}

void rennit( struct OverSharedData * bill){
    
    
    bill->rep = (struct SharedData**)malloc(sizeof(struct SharedData*)*consumerthreads);
    int on =0;
    for (on=0; on<consumerthreads; on++) {
        *(bill->rep+on) = (struct SharedData *)malloc(sizeof(struct SharedData));
        initialize(*(bill->rep + on), on);
        
    }
    
}

void* Producer(void *arg)
{
    struct OverSharedData *	de = (struct OverSharedData *)arg;
    
    printf("\nEntiered Producers\n");
    char *buffer = malloc(1000);
    if ( !buffer )
        return NULL;
    
    FILE *f = fopen(bookorders, "r");
    int c, z;
    
    if ( f ) for (;;)
    {
        for ( z = 0; z < 999 && (c = fgetc(f)) != EOF && c != '\n'; ++z )
            buffer[z] = c;
        
        if ( z == 0 && c == EOF )
            break;
        
        buffer[z] = 0;
        
        char const delim[] = "|";
        char *rpr;
        char *title = strtok_r(buffer, delim, &rpr);
        if ( title[0] )
        {
            ++title;
            title[strlen(title) - 1] = 0;
        }
        
        
        char *ptr = strtok_r(NULL, delim, &rpr);
        double cost = ptr ? atof(ptr) : 0;
        
        ptr = strtok_r(NULL, delim, &rpr);
        int id = ptr ? atoi(ptr) : 0;
        
        char * cat_name = strtok_r(NULL, delim, &rpr);
        int thread_number = translate(cat_name);
        
        printf("Title: %s\n, cat_name:%s thread number: %d\n", title, cat_name, thread_number);
        NodePtr sert = NULL;
        sert = NodalCreate(title, cost, id, cat_name, 0);
        
        struct SharedData * d = *((de->rep)+thread_number);
        int ge = -1;
        while (fifo_len(d->queue)==10) {
            ge++;
            if(ge==0){
                printf("Producer waits, buffer is full\n");}
            continue;
        }
        if(ge>-1){
            printf("Producer resumes, buffer has space\n");
        }
        
        sem_wait( &d->empty_count );
        sem_wait( &d->use_queue );
        
        fifo_add(d->queue, sert);
        
        sem_post( &d->use_queue );
        sem_post( &d->full_count );
        
    }
    int sum = 0;
    
    
    
    NodePtr no = NodalCreate("ABHISHEKSAHAEMPTY", 1.2, 2, "ABHISHEKSAHAEMPTY", 0);
    for(sum=0; sum<consumerthreads; sum++){
        struct  SharedData * d= *((de->rep)+sum);
        fifo_add(d->queue, no);
        
    }
    
    
    free(buffer);
    fclose(f);
    int i =9;
    pthread_mutex_t rew;
    pthread_mutex_init(&rew, 0);
    
    for (i=0; i<100000000; i++) {
      
        
        
    }
    /*
    for (i=0; i<consumerthreads; i++) {
        struct SharedData * d= *((de->rep)+i);
        pthread_mutex_lock(&rew);
        pthread_cond_wait(&d->buffer_empty, &rew);
    }*/
    
    
    printf("Revenue: %f\n", revenue);
   
    return NULL;
}

void printout(){
    
    FILE * f = fopen(dtabase, "r");
    FILE* fp = fopen("finalorder.txt", "w");
    char * buffer = (char *)malloc(1000);
    char c = fgetc(f);
    int i = 0;
    while(c!=EOF){
        while(c!='\n'){
            *(buffer+i) = c;
            i++;
            c = fgetc(f);
        }
        
        char delim[2] = "|";
                
        
        char * first_name = strtok(buffer, delim);
        first_name = first_name +1;
        int leng = (int)strlen(first_name);
        *(first_name+leng-1) = '\0';
        int ident = -1; double credit = -1;
        ident = atoi(strtok(NULL, delim));
        credit = atof(strtok(NULL, delim));
        
      
        HashBucket * entry = (HashBucket*)malloc(sizeof(HashBucket));

        HASH_FIND_INT(users, &ident, entry);
        fprintf(fp, "START CUSTOMER INFO\n");
        fprintf(fp, "Customer Name: %s\n", entry->uzer->name);
        fprintf(fp, "Customer ID: %d\n", ident);
        fprintf(fp, "Remaining credit balance: %f\n", entry->uzer->wallet);
        fprintf(fp, "\nSUCCESSFUL ORDERS\n");
        NodePtr nop = NULL;
        fifo_t * good = entry->uzer->accept;
        while (fifo_len(good)!=0) {
            nop = fifo_remove(good);
            fprintf(fp, "%s | %f | %f \n", nop->title, nop->cost, nop->balance);
        }
        fprintf(fp, "\nREJECTED ORDERS\n");
        fifo_t * bad = entry->uzer->reject;
        while (fifo_len(bad)!=0) {
            nop = fifo_remove(bad);
            fprintf(fp, "%s | %f \n", nop->title, nop->cost);
        }
        fprintf(fp, "END CUSTOMER INFO\n");
        i = 0;
        *buffer = '\0';
        c = fgetc(f);
    }
    fclose(fp);
    rewind(f);
    fclose(f);
    

}

void* Consumer(void *arg){
    
    struct SharedData * d = (struct SharedData*)arg;
    printf("Entered Consumer %d\n", d->da);
    HashBucket * u_lookup;
    
    double current_wallet = -.1;
    int i = 0;
    for(;;) {
        
        fifo_t * remy = d->queue;
        
        if (fifo_len(remy)==0) {
            if(i==0){
                printf("Consumer: %d waits, buffer empty\n", d->da);
                i++;
                }
            continue;
        }
        
        if(i>0){
            printf("Consumer resumes, buffer has value\n");}
        i=0;
        sem_wait( &d->full_count );
		sem_wait( &d->use_queue );
        
        
        NodePtr nop = (NodePtr)fifo_remove(remy);
        printf("Con-title: %s ThreadNumba: %d\n", nop->title, d->da);
        if (strcmp(nop->title, "ABHISHEKSAHAEMPTY")==0) {
            sem_post( &d->use_queue );
            sem_post( &d->empty_count );
            pthread_cond_broadcast(&d->buffer_empty);
            return NULL;
           
        }
        
        
        pthread_mutex_lock(&grill);
        HASH_FIND_INT(users, &(nop->ID), u_lookup);
        current_wallet = u_lookup->uzer->wallet;
        if ((nop->cost)>current_wallet) {
            fifo_add(u_lookup->uzer->reject, nop);
        }
        else{
            revenue += nop->cost;
            u_lookup->uzer->wallet  = u_lookup->uzer->wallet - nop->cost;
            nop->balance =  u_lookup->uzer->wallet;
            fifo_add(u_lookup->uzer->accept, nop);
            
        }
        
        pthread_mutex_unlock(&grill);
        sem_post( &d->use_queue );
		sem_post( &d->empty_count );
	}
    
    
    
    
    
}

int main(int argc, const char * argv[])
{
    pthread_cond_init(&finis, 0);
    revenue = 0;
    databases(argv[1]); /* Takes care of setting up the database*/
    categories(argv[2]); /*Takes care of setting up the book categories*/
    bookorders = argv[3];
    struct OverSharedData* remp = (struct OverSharedData*)malloc(sizeof(struct OverSharedData));
    rennit(remp);
    struct SharedData * d = *(remp->rep + 0);
    
    pthread_mutex_init( &grill, 0 );
    int z = 0;
    pthread_t Consumer_Threads[consumerthreads];
    for (z=0; z<consumerthreads; z++) {
        remp->rop = z;
        d = *(remp->rep + z);
        d->da = z;
        pthread_create((Consumer_Threads+z), 0, Consumer, d);
        
        
    }
    //sleep(2);
    pthread_t Produc;
    pthread_create(&Produc, 0, Producer, remp);
    
    
    
    
    
    pthread_join(Produc, NULL);
    for (z = 0; z<consumerthreads; z++) {
        pthread_join(Consumer_Threads[z], NULL);
    }
    
    //pthread_join() HERE!!!!
    //print final results
    printf("\nPRINT INT RESULTS\n");
    printout();
    
    return 0;
}








NodePtr NodalCreate(char * title, double cost, int ID, char * category, double balance){
    NodePtr nod = (NodePtr)malloc(sizeof(Node));
    char * nop = (char *)malloc(strlen(title)+1);
    strcpy(nop, title);
    nod->title = nop;
    nod->cost = cost;
    nod->ID = ID;
    nod->balance = balance;
    char * nip = (char *)malloc(strlen(category)+1);
    strcpy(nip, category);
    nod->category = nip;
    return nod;
}

void prep(sbuf_t * rate){
    fifo_t* noop = fifo_new();
    rate->queue = noop;
    sem_init(&(rate->full), 0, 0);
    sem_init(&(rate->empty), 0, 100);
    pthread_mutex_init(&(rate->mutex), NULL);
}


int translate(char * string){
    HashBucket * str;
    HASH_FIND_STR(categores, string, str);
    return str->ID;
}



void categories(char * cata){
	
    FILE * f = fopen(cata, "r");
    
    
    char * buffer = (char *)malloc(1000);
    int i = 0;
    while (fscanf(f, "%s", buffer)!=EOF) {
        HashBucket * q = malloc(sizeof(HashBucket));
        q->ID = i;
        q->key = malloc(strlen(buffer+1));
        strcpy(q->key, buffer);
        
        HASH_ADD_STR(categores, key, q);
        i++;
    }
    consumerthreads = i;
    
    fclose(f);
    return;
}




void databases(char * database){
    dtabase = database;
    FILE * f = fopen(database, "r");
    char * buffer = (char *)malloc(1000);
    char c = fgetc(f);
    int i = 0;
    while(c!=EOF){
        while(c!='\n'){
            *(buffer+i) = c;
            i++;
            c = fgetc(f);
        }
        
        char delim[2] = "|";
               
        
        char * first_name = strtok(buffer, delim);
        first_name = first_name +1;
        int leng = (int)strlen(first_name);
        *(first_name+leng-1) = '\0';
        int ident = -1; double credit = -1;
        ident = atoi(strtok(NULL, delim));
        credit = atof(strtok(NULL, delim));
        
        PacketPtr tomp = PCreate(first_name, credit);
        HashBucket * entry = (HashBucket*)malloc(sizeof(HashBucket));
        entry->uzer = tomp;
        entry->ID = ident;
        HASH_ADD_INT(users, ID, entry);
        
        i = 0;
        *buffer = '\0';
        c = fgetc(f);
    }
    rewind(f);
    fclose(f);
    
}

PacketPtr PCreate(char *name, double wallet){
    PacketPtr nop = (PacketPtr)malloc(sizeof(Packet));
    char * tempname = (char *)malloc(sizeof(char)*(strlen(name)+1));
    strcpy(tempname, name);
    nop->name = tempname;
    nop->wallet = wallet;
    nop->accept = fifo_new();
    nop->reject = fifo_new();
    return nop;
}






