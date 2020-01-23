
/*---------------------------------------------------------------------------------------
--      Source File:            lab2.c
--
--      Functions:              main
--                              consumer_func
--                              producer_func
--                              usage
--
--      Date:                   January 23, 2020
--
--      Revisions:              N/A
--                                      
--      Designer:               Derek Leung
--                              
--      Programmer:             Derek Leung
--
--      Notes:
--      This program simulates a communication process between the producer and consumer
--      by utilizing pthreads.   
--
--      To compile the application:
--                      
--            gcc -Wall -o lab2 lab2.c -lpthread

---------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define NUM_UPDATES 3
#define OPTIONS		"?s:i:d:"

// Globals
typedef struct 
{
    char* string;
    int int1;
    double double1;
    int consumer;
} ThreadInfo;

//Global thread variables
pthread_mutex_t consumer_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t producer_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t producer_signal, consumer_signal;

int count;

//Default input
char* string_default[NUM_UPDATES] = {"first", "second", "third"}; 
int int_default[NUM_UPDATES] = {1,2,3};
double double_default[NUM_UPDATES] = {1.2,2.3,3.4};

// Function Prototypes
void usage (char **);
void* consumer_func(void * info_ptr);
void* producer_func(void * info_ptr);

int main (int argc, char **argv)
{
    ThreadInfo *Info_Ptr;
    pthread_t producerThread, consumerThread;

    int opt;
    count = 1;

    if ((Info_Ptr = malloc (sizeof (ThreadInfo))) == NULL)
    {
        perror ("malloc");
        exit (1);
    }

    Info_Ptr->string = string_default[0];
    Info_Ptr->int1 = int_default[0];
    Info_Ptr->double1 = double_default[0];

    Info_Ptr->consumer = 1;
  // Process command line options
    while ((opt = getopt (argc, argv, OPTIONS)) != -1)
    {
        switch (opt)
        {
            case 's': 
              Info_Ptr->string = optarg;
            break;
                
            case 'i':
              Info_Ptr->int1 = atoi(optarg);  
            break;
          
            case 'd':
              Info_Ptr->double1 = atof (optarg);
            break;
                
            default:
              case '?':
              usage (argv);
            break;
        }
    }

   // Spawn a new thread
    pthread_create (&consumerThread, NULL, consumer_func, (void*)Info_Ptr);
    pthread_create (&producerThread, NULL, producer_func, (void*)Info_Ptr);
    pthread_join (producerThread, NULL);
    pthread_join (consumerThread, NULL);
    free (Info_Ptr);
    exit (0);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: consumer_func
--
-- DATE: January 23, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Derek Leung
--
-- PROGRAMMER: Derek Leung
--
-- INTERFACE: void consumer_func(void* info_ptr)
--              void* info_ptr: a ThreadInfo struct passed into the thread function.
--
-- RETURNS: void.
--
-- NOTES:
-- This function handles the consumer thread. Will read the ThreadInfo struct for data and print it out to stdout and
-- then wait for the appropriate signal from producer to read again.
----------------------------------------------------------------------------------------------------------------------*/
void* consumer_func(void* info_ptr)
{
    pthread_mutex_lock(&consumer_lock);
    ThreadInfo *info = info_ptr;
    for(int i=0; i < NUM_UPDATES; i++)
    {
        printf ("\nConsumer Reading struct data...\nString1: %s, Integer1: %d, double1: %f\n", info->string, info->int1, info->double1); 
        if(i == NUM_UPDATES -1)
        {
            pthread_cond_signal(&producer_signal);
            pthread_mutex_unlock(&consumer_lock);
        } else {
            pthread_cond_signal(&producer_signal);
            pthread_cond_wait(&consumer_signal, &producer_lock);
        }
        
    }
    return NULL;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: producer_func
--
-- DATE: January 23, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Derek Leung
--
-- PROGRAMMER: Derek Leung
--
-- INTERFACE: void producer_func(void* info_ptr)
--              void* info_ptr: a ThreadInfo struct passed into the thread function.
--
-- RETURNS: void.
--
-- NOTES:
-- This function handles the producer thread. Will update the ThreadInfo struct with new data and signal the consumer
-- thread to run again.
----------------------------------------------------------------------------------------------------------------------*/
void* producer_func(void* info_ptr)
{
    pthread_mutex_lock(&producer_lock);
    ThreadInfo *info = info_ptr;
    for(int i=1; i< NUM_UPDATES; i++)
    {
        printf("\nProducer updating struct data to string = %s, int = %d, double = %f\n", string_default[count], int_default[count], double_default[count]);
        info->string = string_default[count];
        info->int1 = int_default[count];
        info->double1 = double_default[count];
        if(++count == NUM_UPDATES)
        {
            pthread_cond_signal(&consumer_signal);
            pthread_mutex_unlock(&producer_lock);
        } else {
            pthread_cond_signal(&consumer_signal);
            pthread_cond_wait(&producer_signal, &consumer_lock);
        }
        
    }
    return NULL;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: usage
--
-- DATE: January 23, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Derek Leung
--
-- PROGRAMMER: Derek Leung
--
-- INTERFACE: void usage (char **argv)
--              char **argv: used for the program name
--
-- RETURNS: void.
--
-- NOTES:
-- This function handles printing the usage information of this application.
----------------------------------------------------------------------------------------------------------------------*/
void usage (char **argv)
{
      fprintf(stderr, "Usage: %s -s <string> -i [integer] -d [double]\n", argv[0]);
      exit(1);
}
