#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void start_routine() {

  printf("hello from child thread!\n");
  exit(0);
}

int main() {

  thread_create(start_routine);

  thread_join();
  
  return 0;
}