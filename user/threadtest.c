#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int x = 0;
lock_t lk;

void inc() {
  for (int i = 0; i < 10000; i++) {
    lock_acquire(&lk);
    x++;
    lock_release(&lk);
  }
}

void start_routine() {

  inc();

  exit(0);
}

int main() {
  lock_init(&lk);

  thread_create(start_routine);

  inc();

  thread_join();

  printf("%d\n", x);
  
  return 0;
}