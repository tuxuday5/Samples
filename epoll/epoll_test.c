#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/epoll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NAMED_FIFO "aFifo"

static void set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl()");
    return;
  }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl()");
  }
}

void errExit(char *msg) {
  perror(msg);
  exit(-1);
}

void printArgs(char **argv,char **env) {
  for(int i=0;argv[i];i++)
    printf("argv[%d]=%s\n",i,argv[i]);

  for(int i=0;env[i];i++)
    printf("env[%d]=%s\n",i,env[i]);
}

short int own_isspace(char c) {
  if(c==' ' || c=='\t')
    return 1;
  else
    return 0;
}

char *ltrim(char *s)
{
    while(own_isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(own_isspace(*--back));
    *(back+1) = '\0';
    return s;
}


void PrintNos(short int max,char *name) {
  int fifo_fd,rVal;
  int bSize=2;
  char buffer[bSize];

  fifo_fd = open(NAMED_FIFO,O_RDONLY);
  if(fifo_fd<0)
    errExit("open");

  for(short int i=0;i<max;i++) {
    rVal = read(fifo_fd,buffer,bSize);
    if(rVal != bSize)
      errExit("read");
    printf("%03d\n",i);
  }
}

int main(int argc, char *argv[],char *env[]) {
  //int pipe_fds_child_stdin[2] ;
  int pipe_fds_child_stdout[2] ;
  pid_t child_id ;

  //if( pipe(pipe_fds_child_stdin) < 0 )
  //  errExit("pipe");

  if( pipe(pipe_fds_child_stdout) < 0 )
    errExit("pipe");

  child_id = fork();

  if( child_id > 0 ) {
    const int MAX_POLL_FDS = 2;
    const int BUF_SIZE = 4;

    size_t readSize;
    char buf[BUF_SIZE],*b;
    int status;

    int epoll_fd;
    int nfds ;
    struct epoll_event e_e, e_events[MAX_POLL_FDS];

    memset(e_events,'\0',sizeof(e_events));
    memset(&e_e,'\0',sizeof(e_e));
    //close(pipe_fds_child_stdin[0]);
    close(pipe_fds_child_stdout[1]);

    epoll_fd = epoll_create1(0);
    if(epoll_fd < 0)
      errExit("epoll_create1");

    e_e.data.fd = pipe_fds_child_stdout[0];
    e_e.events  = EPOLLIN;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fds_child_stdout[0], &e_e) < 0)
      errExit("epoll_ctl");

    while(1) {
      nfds = epoll_wait(epoll_fd, e_events,MAX_POLL_FDS,-1);
      if( nfds < 0)
        errExit("epoll_wait");

      for(int i=0;i<nfds;i++) {
        if( e_events[i].data.fd == pipe_fds_child_stdout[0]) {
          if( e_events[i].events & EPOLLIN) {
            readSize = read(pipe_fds_child_stdout[0],buf,BUF_SIZE);
            if( readSize ) {
              write(STDOUT_FILENO,buf,BUF_SIZE);
            } else if(readSize == 0) { // eof
              errExit("readSize=0");
            } else if(readSize < 0) {
              errExit("read");
            }
          } else if( e_events[i].events & EPOLLHUP) {
            printf("got EPOLLHUP on pipefd\n");
            wait(&status);
            exit(0);
          } else {
            errExit("Unexpected event flag returned by epoll_wait on waited fd");
          }
        } else  {
          errExit("epoll_wait returned non-awaited fd");
        }
      }
    }
  } else if( child_id == 0 ) {
    close(0);
    close(1);
    close(2);
    //close(pipe_fds_child_stdin[1]);
    close(pipe_fds_child_stdout[0]);

    //dup2(pipe_fds_child_stdin[0],0);
    dup2(pipe_fds_child_stdout[1],1);
    dup2(pipe_fds_child_stdout[1],2);

    execvpe(argv[1],&(argv[1]),env);
    //PrintNos(100,"P");
    //errExit("execvp");
  } else {
    errExit("fork");
  }
}
