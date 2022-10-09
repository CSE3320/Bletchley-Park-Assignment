#include "include/crypto.h"
#include "schedule.h"
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "clock.h"

#define MAX_NUM_THREADS 1024 
#define MAX_FILENAME_LENGTH 255
#define BUFFER_SIZE 148

static char * message_buffer[BUFFER_SIZE];
static int count ;
static int running = 1 ;

pthread_t message_receiver_tid ;

static void handleSIGUSR2( int sig )
{
  printf("Time to shutdown\n");
  running = 0 ;
}

int insertMessage( char * message )
{
  assert( count < BUFFER_SIZE && "Tried to add a message to a full buffer");

  strncpy( message_buffer[count] , message, MAX_FILENAME_LENGTH ); 
  count++;
  
  return 0;
}

int removeMessage( char *message )
{
  assert( count && "Tried to remove a message from an empty buffer");
  strncpy( message, message_buffer[count], MAX_FILENAME_LENGTH ); 
  count--;

  return 0;
}

static void * tick ( void ) 
{
   printf("Tick!\n") ;
   return NULL ;
}

void * receiver_thread( void * args )
{
  while( running )
  {
    char * message_file = retrieveReceivedMessages( );

    if( message_file )
    {
      insertMessage( message_file ) ;
    }
  }
}

void * decryptor_thread( void * args )
{
  while( running )
  {
    //decrypt files
  }
}

int main( int argc, char * argv[] )
{
    pthread_t tid[ MAX_NUM_THREADS ] ;

    // initialize the message buffer
    int i ;
    for( i = 0; i < BUFFER_SIZE; i++ )
    {
        message_buffer[i] = ( char * ) malloc( MAX_FILENAME_LENGTH ) ;
    }

    count = 0 ;

    struct sigaction act;
    memset ( & act, '\0', sizeof( act ) ) ;
    act . sa_handler = & handleSIGUSR2 ;

    if ( sigaction( SIGUSR2, &act, NULL ) < 0 )  
    {
      perror ( "sigaction: " ) ;
      return ;
    }

    initializeClock( ONE_SECOND ) ;
    registerWithClock( tick ) ;

    initializeSchedule( "schedule.txt" ) ;

    pthread_create( &message_receiver_tid, NULL, receiver_thread, NULL ) ;

    startClock( ) ;

    while( running ) ;

    stopClock( ) ;

    pthread_join( message_receiver_tid, NULL ) ;

    for( i = 0; i < BUFFER_SIZE; i++ )
    {
        free( message_buffer[i] ) ;
    }
    freeSchedule( ) ;

    return 0 ;
}
