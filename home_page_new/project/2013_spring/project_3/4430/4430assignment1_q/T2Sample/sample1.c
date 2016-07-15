/*
 * This is sample code displaying the multi-thread programming
 * for CSCI4430
 */
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>

void* pthread_prog(void * args)
{
    int threadId = *(int*) args;
    sleep( 3 - threadId );
    printf( "This is worker thread %d\n",threadId );
	return 0;//pthread_exit(NULL); is also OK
}

int main(int argc, char** argv)
{
	pthread_t thread[3];
	//int private = 1;
	int i;
	int arg[3];
	arg[0] = 1;
	arg[1] = 2;
	arg[2] = 3;
	for(i = 0; i < 3; i++)
	{
		int ret_val = pthread_create( &thread[i], NULL, pthread_prog, &arg[i] );
		//int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
		//The  pthread_create()  function  starts  a  new  thread  in the calling
       //process.  The new thread starts execution by invoking  start_routine();
       //arg is passed as the sole argument of start_routine().
       //On  success,  pthread_create() returns 0; on error, it returns an error
       //number, and the contents of *thread are undefined.
	}
	
	
	for(i = 0; i < 3; i++)
	{
		pthread_join( thread[i], NULL );//wait untill all work threads are finish
		// int pthread_join(pthread_t thread, void **retval);
		//The pthread_join() function waits for the thread specified by thread to
		//terminate.  If that thread has already terminated, then  pthread_join()
		//returns immediately.  The thread specified by thread must be joinable.
		//On  success,  pthread_join()  returns  0; on error, it returns an error
        //number.
	}
	
	printf("This is master thread\n");
	
	return 0;
}
