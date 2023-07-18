/*
 *  Solution to Producer Consumer Problem
 *  Using Ptheads, a mutex and condition variables
 */

/*
    In this version the buffer is a single number.
    The producer is putting numbers into the shared buffer
    (in this case sequentially)
    And the consumer is taking them out.
    If the buffer contains zero, that indicates that the buffer is empty.
    Any other value is valid.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 100000				/* Numbers to produce */
#define BUFFER_SIZE 10				/* Size of the buffer */

pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;
int buffer[BUFFER_SIZE];
int buffer_index = 0;

void* producer(void *ptr) {
    int i;
    int producer_id = *((int *)ptr);

    for (i = 1; i <= MAX; i++) {
        pthread_mutex_lock(&the_mutex);		/* protect buffer */
        while (buffer_index == BUFFER_SIZE)	/* If buffer is full, wait */
            pthread_cond_wait(&condp, &the_mutex);

        buffer[buffer_index++] = i;
        printf("Producer %d wrote %d\n", producer_id, i);
        pthread_cond_signal(&condc);		/* wake up consumer */
        pthread_mutex_unlock(&the_mutex);	/* release the buffer */
    }

    pthread_exit(0);
}

void* consumer(void *ptr) {
    int i;
    int consumer_id = *((int *)ptr);

    for (i = 1; i <= MAX; i++) {
        pthread_mutex_lock(&the_mutex);		/* protect the buffer */
        while (buffer_index == 0)			/* If buffer is empty, wait */
            pthread_cond_wait(&condc, &the_mutex);

        int item = buffer[--buffer_index];
        printf("Consumer %d reads %d\n", consumer_id, item);
        pthread_cond_signal(&condp);		/* wake up consumer */
        pthread_mutex_unlock(&the_mutex);	/* release the buffer */
    }

    pthread_exit(0);
}

int main(int argc, char **argv) {
    int num_producers_consumers;
    printf("Enter the number of producers and consumers: ");
    scanf("%d", &num_producers_consumers);

    pthread_t producers[num_producers_consumers];
    pthread_t consumers[num_producers_consumers];

    // Initialize the mutex and condition variables
    /* What's the NULL for ??? */
    /* The NULL is the attribute parameter and by passing NULL as the attributes object, the function uses the default attributes */
    pthread_mutex_init(&the_mutex, NULL);
    pthread_cond_init(&condc, NULL);		/* Initialize consumer condition variable */
    pthread_cond_init(&condp, NULL);		/* Initialize producer condition variable */

    // Create the producer threads
    for (int i = 0; i < num_producers_consumers; i++) {
        int *producer_id = (int *)malloc(sizeof(int));
        *producer_id = i + 1;
        pthread_create(&producers[i], NULL, producer, (void *)producer_id);
    }

    // Create the consumer threads
    for (int i = 0; i < num_producers_consumers; i++) {
        int *consumer_id = (int *)malloc(sizeof(int));
        *consumer_id = i + 1;
        pthread_create(&consumers[i], NULL, consumer, (void *)consumer_id);
    }

    // Wait for the threads to finish
    // Otherwise main might run to the end
    // and kill the entire process when it exits.
    for (int i = 0; i < num_producers_consumers; i++)
        pthread_join(producers[i], NULL);
    for (int i = 0; i < num_producers_consumers; i++)
        pthread_join(consumers[i], NULL);

    // Cleanup -- would happen automatically at the end of the program
    pthread_mutex_destroy(&the_mutex);	/* Free up the_mutex */
    pthread_cond_destroy(&condc);		/* Free up consumer condition variable */
    pthread_cond_destroy(&condp);		/* Free up producer condition variable */

    return 0;
}