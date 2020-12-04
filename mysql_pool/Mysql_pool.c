/*
 * @Author: Mayc
 * @Date: 2020-12-04 14:38:01
 * @LastEditTime: 2020-12-04 15:28:53
 * @FilePath: \testd:\VSC++&GO\ProjectLearning\mysql_pool\Mysql_pool.c
 * @这是一个漂亮的注释头
 */

#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define MYC_DB_SERVER_IP "192.168.249.133"
#define MYC_DB_SERVER_PORT 3306

#define MYC_DB_USERNAME "admin"
#define MYC_DB_PASSWORD "123456"

#define MYC_DB_DEFAULTDB "MYC_DB"

#define SQL_INSERT_TBL_USER "INSERT TBL_USER(U_NAME, U_GENGDER) VALUES('liming', 'man');"
#define SQL_SELECT_TBL_USER "SELECT * FROM TBL_USER;"


#define SQL_DELETE_TBL_USER "CALL PROCEDURE_DELETE_USER('lee');"
#define SQL_INSERT_IMG_USER "INSERT TBL_USER(U_NAME, U_GENGDER, U_IMG) VALUES('liming', 'man', ?);"

#define SQL_SELECT_IMG_USER "SELECT U_IMG FROM TBL_USER WHERE U_NAME='liming';"



#define FILE_IMAGE_LENGTH (64*1024)

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
	if(list == item) list = item->next; 						\
	item->prev = item->next = nullptr;							\
} while(0)



//C U R D --> creat, update, read, delete

int myc_mysql_select(MYSQL* handle){
	// mysql_real_query --> sql
	if(mysql_real_query(handle, SQL_SELECT_TBL_USER, strlen(SQL_SELECT_TBL_USER))){
		printf("mysql_real_query : %s\n", mysql_error(handle));
		return -1;
	}

	// store -->
	MYSQL_RES* res = mysql_store_result(handle);
	if(res == nullptr) {
		printf("mysql_store_query : %s\n", mysql_error(handle));
		return -2;
	}
	
	// rows / fields

	int rows = mysql_num_rows(res);
	printf("rows: %d\n", rows);

	int fields = mysql_num_fields(res);
	printf("fields: %d\n", fields);

	// fetch
	MYSQL_ROW row;
	while((row = mysql_fetch_row(res))){

		int i = 0;
		for(i = 0; i < fields; i++){
			printf("%-10s", row[i]);
		}
		printf("\n");
		
	}
	
	mysql_free_result(res);

	
	return 0;
}


// filename : file name
// buffer : store image data
int read_image(char* filename, char* buffer){
	if(filename == nullptr || buffer == nullptr ) return -1;
	FILE* fp = fopen(filename, "rb");
	if(fp == nullptr){
		printf("fopen failed\n");
		return -2;
	}
	// file size
	// point to tail
	fseek(fp, 0, SEEK_END);
	// file size
	int length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	int size = fread(buffer, 1, length, fp);
	if ( size != length ){
		printf("fread failed: %d\n", size);
		return -3;
	}

	fclose(fp);
	return size;
}


// filename : 
// buffer :
// length :

int write_image(char* filename, char* buffer, int length){
	if(filename == nullptr || buffer == nullptr || length <= 0) return -1;
	FILE* fp = fopen(filename, "wb+");
	if(fp == nullptr){
		printf("fopen failed\n");
		return -2;
	}
	
	int size = fwrite(buffer, 1, length, fp);
	if  ( size != length ){
		printf("fwrite failed: %d\n", size);
		return -3;
	}

	fclose(fp);
	return size;
}

int mysql_write(MYSQL* handle, char* buffer, int length){
	if( handle == nullptr || buffer == nullptr || length <= 0 ) return -1;

	MYSQL_STMT* stmt = mysql_stmt_init(handle);
	int ret = mysql_stmt_prepare(stmt, SQL_INSERT_IMG_USER, strlen(SQL_INSERT_IMG_USER));
	if(ret){
		printf("mysql_stmt_prepare : %s\n", mysql_error(handle));
		return -2;
	}

	MYSQL_BIND param = { 0 };
	param.buffer_type = MYSQL_TYPE_LONG_BLOB;
	param.buffer = nullptr;
	param.is_null = 0;
	param.length = nullptr;

	ret = mysql_stmt_bind_param(stmt, &param);
	if ( ret ) {
		printf("mysql_stmt_bind_param : %s\n", mysql_error(handle));
		return -3;
	}

	ret = mysql_stmt_send_long_data(stmt, 0, buffer, length);
	if( ret ){
		printf("mysql_stmt_send_long_data : %s\n", mysql_error(handle));
		return -4;
	}

	ret = mysql_stmt_execute(stmt);
	if( ret ){
		printf("mysql_stmt_execute : %s\n", mysql_error(handle));
		return -5;

	}

	ret = mysql_stmt_close(stmt);
	if( ret ){
		printf("mysql_stmt_close : %s\n", mysql_error(handle));
		return -6;
	}

	return ret;
	
	
}

int mysql_read(MYSQL* handle, char* buffer, int length){
	if( handle == nullptr || buffer == nullptr || length <= 0 ) return -1;

	MYSQL_STMT* stmt = mysql_stmt_init(handle);
	int ret = mysql_stmt_prepare(stmt, SQL_SELECT_IMG_USER, strlen(SQL_SELECT_IMG_USER));
	if(ret){
		printf("mysql_stmt_prepare : %s\n", mysql_error(handle));
		return -2;
	}

	MYSQL_BIND result = { 0 };
	
	result.buffer_type = MYSQL_TYPE_LONG_BLOB;
	unsigned long total_length = 0;
	result.length = &total_length;	

	ret = mysql_stmt_bind_result(stmt, &result);
	if ( ret ){
		printf("mysql_stmt_bind_result : %s\n", mysql_error(handle));
		return -3;		
	}

	ret = mysql_stmt_execute(stmt);
	if( ret ){
		printf("mysql_stmt_execute : %s\n", mysql_error(handle));
		return -4;
	}

	ret = mysql_stmt_store_result(stmt);
	if( ret ){
		printf("mysql_stmt_store_result : %s\n", mysql_error(handle));
		return -5;
	}

	while (1){
		ret = mysql_stmt_fetch(stmt);
		if (ret != 0 && ret != MYSQL_DATA_TRUNCATED)break;

		int start = 0;
		while(start < (int)total_length){
			result.buffer = buffer + start;
			result.buffer_length = 1;
			mysql_stmt_fetch_column(stmt, &result, 0, start);
			start += result.buffer_length;
		}
	}

	mysql_stmt_close(stmt);
	return total_length;

	
}


struct mysql_nWorker{
	MYSQL mysql;
	int terminate;
	void* user_data;


	
	struct mysql_nManager* manager;
	struct mysql_nWorker* prev;
	struct mysql_nWorker* next;
};

struct mysql_nTask{

	void ( *task_func )(struct mysql_nWorker* worker);
	void* user_data;
	
	//struct mysql_nManager* manager;
	struct mysql_nTask* prev;
	struct mysql_nTask* next;
};



typedef struct mysql_nManager {
	struct mysql_nTask* tasks;
	struct mysql_nWorker* workers;

	pthread_mutex_t mutex;
	pthread_cond_t cond;
} MysqlPool;


static void* nMysqlPoolCallback(void* arg){

	struct mysql_nWorker* worker = (struct mysql_nWorker*) arg;
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

		struct mysql_nTask* task = worker->manager->tasks;
		LIST_REMOVE(task, worker->manager->tasks);
		pthread_mutex_unlock(&worker->manager->mutex);
		task->task_func(worker);
		free(task);
	}
	printf("free\n");
	free(worker);
		
}




//API
int nMysqlPoolCreate(MysqlPool* pool, int numWorkers){
	if ( pool == nullptr ) return -1;
	if ( numWorkers < 1 ) numWorkers = 1;
	memset(pool, 0, sizeof(MysqlPool));


	pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
	memcpy(&pool->cond, &blank_cond,sizeof(pthread_cond_t));

	pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
	memcpy(&pool->mutex, &blank_mutex, sizeof(pthread_mutex_t));
	

	int i = 0;
	for(i = 0; i < numWorkers; i++){
		pthread_t threadid;
		struct mysql_nWorker* worker = (struct mysql_nWorker*)malloc(sizeof(struct mysql_nWorker));
		if(worker == nullptr){
			perror("malloc");
			return -2;
		}
		
		memset(worker, 0, sizeof(struct mysql_nWorker));
		worker->manager = pool;

		if(nullptr == mysql_init(&worker->mysql)){
			printf("mysql_init : %s\n", mysql_error(&worker->mysql));
			mysql_close(&worker->mysql);
		}
		int ret = pthread_create(&threadid, nullptr, nMysqlPoolCallback, worker);
		if(ret) {
			perror("pthread_create");
			free(worker);
			return -3;
		}
		if(!mysql_real_connect(&worker->mysql, MYC_DB_SERVER_IP, MYC_DB_USERNAME, MYC_DB_PASSWORD, 
			MYC_DB_DEFAULTDB, MYC_DB_SERVER_PORT, nullptr, CLIENT_MULTI_STATEMENTS)){
			printf("mysql_connect : %s\n", mysql_error(&worker->mysql));
			mysql_close(&worker->mysql);
		}
		worker->user_data = malloc(sizeof(int));
		*(int*)worker->user_data = i;
		LIST_INSERT(worker, pool->workers);
	}
	
	return 0;
	
}

//API
int nMysqlPoolDestory(MysqlPool* pool, int nWorker){
	struct mysql_nWorker* worker = nullptr;
	for(worker = pool->workers; worker != nullptr; worker = worker->next){
		mysql_close(&worker->mysql);
		worker->terminate= 1;
	}

	pthread_mutex_lock(&pool->mutex);

	pthread_cond_broadcast(&pool->cond);

	pthread_mutex_unlock(&pool->mutex);

	pool->workers = nullptr;
	pool->tasks = nullptr;
	return 0;
}


int nMysqlPoolPushTask(MysqlPool* pool, struct mysql_nTask* task){

	pthread_mutex_lock(&pool->mutex);

	LIST_INSERT(task, pool->tasks);

	pthread_cond_signal(&pool->cond);

	pthread_mutex_unlock(&pool->mutex);

	return 0;
}

#if 1

#define MYSQLPOOL_INIT_COUNT 10
#define TASK_INIT_SIZE 30

void task_entry(struct mysql_nWorker* worker){


	int idx = *(int*) worker->user_data;

	myc_mysql_select(&worker->mysql);


	printf("case: mysql --> read image and write mysql \n");

	char buffer[FILE_IMAGE_LENGTH] = { 0 };
	int length = read_image("/home/mayichao/share/05_mysql/0voice.jpg", buffer);

	if (length < 0) goto Exit;


	mysql_write(&worker->mysql, buffer, length);
	
	printf("case: mysql --> read mysql and write image \n");
	memset(buffer, 0, FILE_IMAGE_LENGTH);
	length = mysql_read(&worker->mysql, buffer, FILE_IMAGE_LENGTH);

	char IIndex[5];
	sprintf(IIndex, "%d", idx);
	char TType[5] = ".jpg";
	char* temp = strcat(IIndex, TType);
	write_image(temp, buffer, length);


Exit:
	//mysql_close(&task->manager->workers->mysql);
	//free(worker);
	return;
}

int main(void) {

	MysqlPool pool = { 0 };
	nMysqlPoolCreate(&pool, MYSQLPOOL_INIT_COUNT);

	int i = 0;
	for(int i = 0; i < TASK_INIT_SIZE; i++){
		struct mysql_nTask* task = (struct mysql_nTask*)malloc(sizeof(struct mysql_nTask));
		if(task == nullptr){
			perror("malloc");
			exit(1);
		}
		
		memset(task, 0, sizeof(struct mysql_nTask));
		
		task->task_func = task_entry;
		//task->user_data = malloc(sizeof(int));
		//*(int*)task->user_data = i;
		nMysqlPoolPushTask(&pool, task);
	}
	getchar();
	nMysqlPoolDestory(&pool, MYSQLPOOL_INIT_COUNT);
	getchar();

}
#endif



