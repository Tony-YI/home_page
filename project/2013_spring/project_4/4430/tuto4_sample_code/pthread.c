/* pthread.c -- demonstrating how to create detached threads and how to ensure
 * threads clean up properly
 *
 * Usage: ./pthread
 * You can see that both threads executes the cleanup routine.
 */

#include <errno.h>    /* needed to access errno */
#include <pthread.h>  /* pthread_* */
#include <stdio.h>    /* perror */
#include <stdlib.h>
#include <unistd.h>   /* sleep */

/* error-handling macro, for explanation of the do {...} while (0) see:
 * http://stackoverflow.com/questions/154136/why-are-there-sometimes-meaningless-do-while-and-if-else-statements-in-c-c-mac
 */
#define show_error(call, err) do { errno=err; perror(call); exit(1); } while (0)

/* Cleanup after thread exits */
void cleanup(void *thread_num) {
  printf("Thread %d cleans up.\n", (int)thread_num);
}

/* Routine for the first thread created */
void *routine1(void *args) {
  /* Register a cleanup routine for current thread, passing 1 in as argument */
  pthread_cleanup_push(cleanup, (void *)1);

  /* Sometimes we encounter error and wish to kill the thread ... 
   * we still runs the cleanup routine */
  pthread_exit(NULL);

  /* push and pop MUST be matched in the same scope */
  pthread_cleanup_pop(1);  /* try compiling without this line */

  return NULL;
}

/* Routine for the second thread created */
void *routine2(void *args) {
  pthread_cleanup_push(cleanup, (void *)2);

  /* We are lucky this time and get to the end of the routine ... */
  if (0) { pthread_exit(NULL); }

  pthread_cleanup_pop(1); /* Cleanup routine runs here */
  return NULL;
}

int main(int argc, char **argv) {
  pthread_t tid1, tid2;
  pthread_attr_t attr;

  /* Set the attributes to create threads as detached */
  int ret = pthread_attr_init(&attr);
  if (ret) { show_error("pthread_attr_init", ret); }
  ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (ret) { show_error("pthread_attr_setdetachstate", ret); }

  /* Create the first thread */
  ret = pthread_create(&tid1, &attr, routine1, NULL);
  if (ret) { show_error("pthread_create", ret); }

  /* Create the second thread */
  ret = pthread_create(&tid2, &attr, routine2, NULL);
  if (ret) { show_error("pthread_create", ret); }

  /* Unnecessary cleanup -- just for demo */
  ret = pthread_attr_destroy(&attr);
  if (ret) { show_error("pthread_attr_destroy", ret); }
  sleep(1);
  return 0;
}

