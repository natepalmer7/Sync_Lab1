#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
#define N 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define LEFT (ph_num+4)%N
#define RIGHT (ph_num+1)%N
 
sem_t mutex;
sem_t S[N];
 
 
void * philospher(void *num);
void take_fork(int);
void put_fork(int);
void test(int);
 
 
int state[N];
int phil_num[N]={0,1,2,3,4};
 
 
int main(){
    int i;
    pthread_t thread_id[N];
    sem_init(&mutex,0,1);
 
 
    for(i=0;i<N;i++)
        sem_init(&S[i],0,0);
    for(i=0;i<N;i++){
        pthread_create(&thread_id[i],NULL,philospher,&phil_num[i]);
        printf("Philosopher %d is thinking\n",i+1);
    }
    for(i=0;i<N;i++)
        pthread_join(thread_id[i],NULL);

    // don't forget to destroy your semaphores (memory leak otherwise)
    sem_destroy(&mutex);
    for(i=0;i<N;i++)
        sem_destroy(&S[i]);
    return 1;

}
 
 
void *philospher(void *num){
    while(1){
        int *i = num;
        sleep(1);
            take_fork(*i);
        sleep(0);
            put_fork(*i);
    }
}
 
 
void take_fork(int ph_num){
    // your line here
    sem_wait(&mutex); // get the mutex semaphore to protect the critical section
    
    state[ph_num] = HUNGRY;
        printf("Philosopher %d is Hungry\n",ph_num+1);
    test(ph_num);
    
    // your line here
    sem_post(&mutex); // let go of the mutex semaphore
    
    // your line here
    sem_wait(&S[ph_num]); // Wait for the S[ph_num] semaphore, which allows the philosopher to start eating
    
    sleep(1);
}
 
 
void test(int ph_num){
    if (state[ph_num] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING){
        state[ph_num] = EATING;
        sleep(2);
            printf("Philosopher %d takes fork %d and %d\n",ph_num+1,LEFT+1,ph_num+1);
            printf("Philosopher %d is Eating\n",ph_num+1);
        
        // your line here
        sem_post(&S[ph_num]); // signal the S[ph_num] semaphore to give permission to the philosopher to start eating
    }
}
 
 
void put_fork(int ph_num){
    // your line here
    sem_wait(&mutex); // get the the mutex semaphore to protect the critical section
    
    state[ph_num] = THINKING;
        printf("Philosopher %d putting fork %d and %d down\n",ph_num+1,LEFT+1,ph_num+1);
        printf("Philosopher %d is thinking\n",ph_num+1);
    test(LEFT);
    test(RIGHT);
    
    // your line here
    sem_post(&mutex); // Release the mutex semaphore
}