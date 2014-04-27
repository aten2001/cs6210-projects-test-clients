#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "rvm.h"

/* proc1 writes some data, commits it, then exits */
int main() 
{
  rvm_t rvm;
  trans_t trans, trans2;
  char* segs[2];

  rvm = rvm_init("rvm_segments_test");
  rvm_destroy(rvm, "testseg");
  segs[0] = (char *) rvm_map(rvm, "testseg", 10000);
  segs[1] = (char *) rvm_map(rvm, "testseg2", 10000);
  trans = rvm_begin_trans(rvm, 2, (void **) segs);
//  rvm_about_to_modify(trans, segs[0], 0, 100);
  sprintf(segs[0], "hello world");
  sprintf(segs[1]+1000, "hello China");
  rvm_commit_trans(trans);
  rvm_truncate_log(rvm);

//  rvm_unmap(rvm, segs[1]);

//  rvm_about_to_modify(trans, segs[0], 200, 100);
  printf("finished second ATM() \n");
//  rvm_abort_trans(trans);

  /* 
  printf("finished rvm_begin_trans()\n");
  rvm_about_to_modify(trans, segs[0], 0, 100);
  printf("finished second ATM \n");
  rvm_about_to_modify(trans2, segs[0], 200, 100);
  printf("finished about_to_modify\n");
  sprintf(segs[0], "hello, world");
     
  rvm_about_to_modify(trans, segs[0], 1000, 100);
  rvm_abort_trans(trans);
  printf("finished the second about_to_modify\n");
  sprintf(segs[0]+1000, "hello, China");
  printf("in the client, segs[0] = %s \n", segs[0]);
   
  rvm_commit_trans(trans);
  rvm_truncate_log(rvm);
*/
  return 0;
}

