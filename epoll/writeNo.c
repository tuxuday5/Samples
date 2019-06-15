#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<errno.h>

#define SLEEP_IN_NANO 100000000
#define LOOP_COUNT 100

void errExit(char *msg) {
  perror(msg);
  exit(-1);
}

int main(int argc,char *argv[],char *env[]) {
  int loop_count = LOOP_COUNT; 
  struct timespec sleepTime ;

  sleepTime.tv_sec = 0;
  sleepTime.tv_nsec = SLEEP_IN_NANO;

  if( argc > 1 ) 
    loop_count = atoi(argv[1]);

  //if(fcntl(1,F_SETFL,O_NONBLOCK) < 0)
  //  errExit("fcntl");

  setvbuf(stdout, NULL, _IONBF, 0);
  for(short i=0;i<loop_count;i++) {
    printf("%03d\n",i);
    //fflush(stdout);
    //fsync(1);
    nanosleep(&sleepTime,NULL);
  }
}
