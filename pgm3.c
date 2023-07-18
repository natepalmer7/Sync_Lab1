#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#define MYSTACKSIZE 40960

/*
The divergence in results from pgm2.c and pgm3.c originates from the way the threads interact with shared resources.

pgm3.c adopts a synchronized method for accessing these shared assets, using mutex locks. This ensures an output that is both orderly and more foreseeable.

On the other hand, pgm2.c lacks the code segments required to lock and unlock the mutex.

Below, the variations between pgm2.c and pgm3.c are further elaborated.


*/

pthread_mutex_t m;

void *odd(void *max)
{
  int i;
  struct timeval tp;

  pthread_mutex_lock(&m); // this gets the the mutex lock
  for (i = 0; i < 10; i++) {
    printf("odd %d\n", i);
    sleep(1);
  }
  pthread_mutex_unlock(&m); // this lets go of the the mutex lock
}

void *even(void *max)
{
  int i;
  struct timeval tp;
 
  pthread_mutex_lock(&m); // this gets the mutex lock
  for (i = 0; i < 10; i++) {
    printf("even %d\n", i);
    sleep(1);
  }
  pthread_mutex_unlock(&m); // lets go of the the mutex lock
}

main()
{
  int max = 50, max1 = 100, max2 = 200, i;
  pthread_attr_t attr;
  pthread_t *th1, *th2;
  void *st1, *st2;
  size_t sz;
  int policy;
  struct timeval tp;

  pthread_mutex_init(&m, NULL);

  pthread_attr_init(&attr);
  st1 = (void *) malloc(MYSTACKSIZE);
  pthread_attr_setstacksize(&attr, MYSTACKSIZE);
  pthread_attr_setstack(&attr, st1, MYSTACKSIZE);

  pthread_attr_getstacksize(&attr, &sz);

  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
  pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

  th1 = (pthread_t *) malloc(sizeof(pthread_t));
  if (pthread_create(th1, &attr, odd, &max1)) {
    perror("error creating the first thread");
    exit(1);
  }
  printf("created the first thread\n");
 
  st2 = (void *)malloc(MYSTACKSIZE);
  pthread_attr_setstacksize(&attr, MYSTACKSIZE);
  pthread_attr_setstack(&attr, st2, MYSTACKSIZE);
  th2 = (pthread_t *) malloc(sizeof(pthread_t));
  if (pthread_create(th2, &attr, even, &max2)) {
    perror("error creating the second thread");
    exit(1);
  }
  printf("created the second thread\n");

  pthread_mutex_lock(&m); // this gets the the mutex lock
  for (i = 0; i < 10; i++) {
    printf("main %d\n", i);
    sleep(1);
  }
  pthread_mutex_unlock(&m); // this lets go of the mutex lock
  pthread_join(*th1, 0);
  pthread_join(*th2, 0);
}

