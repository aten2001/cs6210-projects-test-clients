#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "rvm.h"

/* proc1 writes some data, commits it, then exits */
void proc1() 
{
  rvm_t rvm;
  trans_t trans;
  char* segs[2];

  rvm = rvm_init("rvm_segments");
  rvm_destroy(rvm, "testseg");
  rvm_destroy(rvm, "testseg2");
  segs[0] = (char *) rvm_map(rvm, "testseg", 10000);
  segs[1] = (char *) rvm_map(rvm, "testseg2", 10000);

  printf("in the client, first, segs[0] = %s, segs[1]=%s \n", segs[0], segs[1]);

//  rvm_unmap(rvm, (void *)segs[0]);
  trans = rvm_begin_trans(rvm, 2, (void **) segs);
     
  printf("finished rvm_begin_trans()\n");
  rvm_about_to_modify(trans, segs[0], 0, 100);
  sprintf(segs[0], "hello, world");
  rvm_about_to_modify(trans, segs[1], 1000, 100);
  printf("finished about_to_modify\n");
  sprintf(segs[1]+1000, "hello, China");
     
  printf("finished the second about_to_modify\n");
  printf("in the client, second, segs[0] pointer = %p, segs[1] pointer = %p \n", &segs[0], &segs[1]);
  printf("in the client, second, segs[0] = %s, segs[1] = %s \n", segs[0], segs[1]);
   
  rvm_commit_trans(trans);
 // rvm_truncate_log(rvm);

  printf("first process, after commit in client \n");
  abort();
}


/* proc2 opens the segments and reads from them */
void proc2() 
{
  char* segs[2];
  rvm_t rvm;
     
  rvm = rvm_init("rvm_segments");

  segs[0] = (char *) rvm_map(rvm, "testseg", 10000);
  if(strcmp(segs[0], "hello, world")) {
    fprintf(stderr, "1st: A second process did not find what the first had written.\n");
    exit(EXIT_FAILURE);
  }
  segs[1] = (char *) rvm_map(rvm, "testseg2", 10000);
  if(strcmp(segs[1]+1000, "hello, China")) {
    fprintf(stderr, "2nd: A second process did not find what the first had written.\n");
    exit(EXIT_FAILURE);
  }
}


int main(int argc, char **argv)
{
  int pid;
  pid = fork();
  if(pid < 0) {
    perror("fork");
    exit(2);
  }
  if(pid == 0) {
    proc1();
    exit(EXIT_SUCCESS);
  }

  waitpid(pid, NULL, 0);

  proc2();

  printf("Ok\n");

  return 0;
}
