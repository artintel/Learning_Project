#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define nullptr NULL

#define THREAD_COUNT 10

pthread_mutex_t mutex;
pthread_spinlock_t spinlock;

int inc(int* value, int add){
	int old;

	__asm__ volatile(
		"lock; xaddl %2, %1;"
		: "=a"(old)
		: "m" (*value), "a"(add)
		: "cc", "memory"
	);
	return old;
}

void* thread_callback(void* arg){
	int* pcount = (int*)arg;
	int i = 0;
	while(i++ < 100000){
		//(*pcount)++;
#if 0

#elif 0
		pthread_mutex_lock(&mutex);
		(*pcount)++;
		pthread_mutex_unlock(&mutex);

#elif 0
		pthread_spin_lock(&spinlock);

		(*pcount)++;
		
		pthread_spin_unlock(&spinlock);

#else
	inc(pcount, 1);

#endif
		usleep(30);
	}
}

int main(){

	pthread_t threadid[THREAD_COUNT] = { 0 };

	pthread_mutex_init(&mutex, nullptr);
	pthread_spin_init(&spinlock, PTHREAD_PROCESS_SHARED);

	
	int i = 0;
	int count = 0;
	for(i = 0; i < THREAD_COUNT; i++){
		pthread_create(&threadid[i], nullptr, thread_callback, &count);
	}
	for(i = 0; i < 100; i++){
		printf("count: %d\n", count);
		sleep(1);
	}
}


