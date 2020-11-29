#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>


#define nullptr NULL


#define LIST_INSERT(item, list) do {	\
	item->prev = nullptr;		  		\
	item->next = list;		      		\
	if((list)!=nullptr) (list)->prev = item;	\
	(list) = item;				  		\
} while(0)



#define LIST_REMOVE(item, list) do {	\
	if(item->prev != nullptr) item->prev->next = item->next;	\
	if(item->next != nullptr) item->next->prev = item->prev;	\
	if(list == item) list = item->next;							\
	item->prev = item->next = nullptr;							\
} while(0)



struct nTask{
	void ( *task_func )(struct nTask* task);
	void* user_data;

	struct nTask* prev;
	struct nTask* next;
	
};

struct nWorker{
	pthread_t threadid;
	int terminate;
	struct nManager* manager;

	
	struct nWorker* prev;
	struct nWorker* next;
};

typedef struct nManager{
	struct nTask* tasks;
	struct nWorker* workers;

	pthread_mutex_t mutex;
	pthread_cond_t cond;
} ThreadPool;


// callback != task
static void* nThreadPoolCallback(void* arg){
	//printf("PoolCallback\n");

	struct nWorker* worker = (struct nWorker*) arg;
	while(1){
		pthread_mutex_lock(&worker->manager->mutex);
		while(worker->manager->tasks == nullptr){
			if(worker->terminate) break;
			pthread_cond_wait(&worker->manager->cond, &worker->manager->mutex);
		}
		if(worker->terminate){
			pthread_mutex_unlock(&worker->manager->mutex);
			break;
		}
		struct nTask* task = worker->manager->tasks;
		LIST_REMOVE(task, worker->manager->tasks);
		pthread_mutex_unlock(&worker->manager->mutex);

		task->task_func(task);
	}
	free(worker);
}

// API
int nThreadPoolCreate(ThreadPool* pool, int numWorkers){
	if ( pool == nullptr ) return -1;
	if ( numWorkers < 1 ) numWorkers = 1;
	memset(pool, 0, sizeof(ThreadPool));

	pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
	memcpy(&pool->cond, &blank_cond, sizeof(pthread_cond_t));

	//pthread_mutex_init(&pool->mutex, nullptr);
	pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
	memcpy(&pool->mutex, &blank_mutex, sizeof(pthread_mutex_t));
	
	int i = 0;
	for(i = 0; i < numWorkers; i++){
		struct nWorker* worker = (struct nWorker*)malloc(sizeof(struct nWorker));
		if (worker == nullptr){
			perror("malloc");
			return -2;
		}
		memset(worker, 0, sizeof(struct nWorker));
		worker->manager = pool;

		int ret = pthread_create(&worker->threadid, nullptr, nThreadPoolCallback, worker);
		if(ret) {
			perror("pthread_create");
			free(worker);
			return -3;
		}
		LIST_INSERT(worker, pool->workers);
	}
	
	//success
	return 0;
}

//API
int nThreadPoolDestory(ThreadPool* pool, int nWorker){
	struct nWorker* worker = nullptr;
	for(worker = pool->workers; worker != nullptr; worker = worker->next){
		worker->terminate;
	}

	pthread_mutex_lock(&pool->mutex);

	pthread_cond_broadcast(&pool->cond);
	
	pthread_mutex_unlock(&pool->mutex);

	pool->workers = nullptr;
	pool->tasks = nullptr;
	return 0;
}

//API
int nThreadPoolPushTask(ThreadPool* pool, struct nTask* task){

	pthread_mutex_lock(&pool->mutex);

	LIST_INSERT(task, pool->tasks);

	pthread_cond_signal(&pool->cond);
	
	pthread_mutex_unlock(&pool->mutex);
	
}

#if 1

#define THREADPOOL_INIT_COUNT 20
#define TASK_INIT_SIZE 10000

void task_entry(struct nTask* task){
	//struct nTask* task = (struct nTask*) task;
	int idx = *(int*)task->user_data;

	printf("idx: %d\n", idx);
	free(task->user_data);
	free(task);
}

int main(void){
	ThreadPool pool = { 0 };
	nThreadPoolCreate(&pool, THREADPOOL_INIT_COUNT);
	//pool --> memset();

	int i = 0;
	//printf("for loop before\n");
	for(int i = 0; i < TASK_INIT_SIZE; i++){
		//printf("for loop in\n");
		struct nTask* task = (struct nTask*)malloc(sizeof(struct nTask));
		if(task == nullptr){
			perror("malloc");
			exit(1);
		}
		memset(task, 0, sizeof(struct nTask));
		task->task_func = task_entry;
		task->user_data = malloc(sizeof(int));
		*(int*)task->user_data = i;


		nThreadPoolPushTask(&pool, task);
	}
	//printf("for loop behind\n");
	getchar();
}
#endif








