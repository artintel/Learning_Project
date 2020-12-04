#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

#define MYC_DB_SERVER_IP "192.168.249.133"
#define MYC_DB_SERVER_PORT 3306

#define MYC_DB_USERNAME "admin"
#define MYC_DB_PASSWORD "123456"

#define MYC_DB_DEFAULTDB "MYC_DB"

#define SQL_INSERT_TBL_USER "INSERT TBL_USER(U_NAME, U_GENGDER) VALUES('liudehua', 'man');"
#define SQL_SELECT_TBL_USER "SELECT * FROM TBL_USER;"


#define SQL_DELETE_TBL_USER "CALL PROCEDURE_DELETE_USER('lee');"
#define SQL_INSERT_IMG_USER "INSERT TBL_USER(U_NAME, U_GENGDER, U_IMG) VALUES('liudehua', 'man', ?);"

#define SQL_SELECT_IMG_USER "SELECT U_IMG FROM TBL_USER WHERE U_NAME='liudehua';"



#define FILE_IMAGE_LENGTH (64*1024)

#define nullptr NULL

//C U R D --> creat, update, read, delete

int myc_mysql_select(MYSQL* handle){
	// mysql_real_query --> sql
	if(mysql_real_query(handle, SQL_SELECT_TBL_USER, strlen(SQL_SELECT_TBL_USER))){
		printf("ysql_real_query : %s\n", mysql_error(handle));
		return -1;
	}

	// store -->
	MYSQL_RES* res = mysql_store_result(handle);
	if(res == nullptr) {
		printf("ysql_store_query : %s\n", mysql_error(handle));
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

int main(){
	MYSQL mysql;

	if(nullptr == mysql_init(&mysql)){
		printf("mysql_init : %s\n", mysql_error(&mysql));
		return -1;
	}

	if(!mysql_real_connect(&mysql, MYC_DB_SERVER_IP, MYC_DB_USERNAME, MYC_DB_PASSWORD, 
		MYC_DB_DEFAULTDB, MYC_DB_SERVER_PORT, nullptr, 0)){
		printf("mysql_read_connect : %s\n", mysql_error(&mysql));
		goto Exit;
		
	}

	// mysql --> insert
	printf("case: mysql --> insert \n");
#if 0	
	if(mysql_real_query(&mysql, SQL_INSERT_TBL_USER, strlen(SQL_INSERT_TBL_USER))){
		printf("ysql_real_query : %s\n", mysql_error(&mysql));
		goto Exit;
	}
#endif

	myc_mysql_select(&mysql);

	//MYSQL --> DELETE


	printf("case: mysql --> delete \n");

#if 0	
	if(mysql_real_query(&mysql, SQL_DELETE_TBL_USER, strlen(SQL_DELETE_TBL_USER))){
		printf("ysql_real_query : %s\n", mysql_error(&mysql));
		goto Exit;
	}
#endif

	myc_mysql_select(&mysql);


	printf("case: mysql --> read image and write mysql \n");

	char buffer[FILE_IMAGE_LENGTH] = { 0 };
	int length = read_image("/home/mayichao/share/05_mysql/0voice.jpg", buffer);

	if (length < 0) goto Exit;


	mysql_write(&mysql, buffer, length);
	
	printf("case: mysql --> read mysql and write image \n");
	memset(buffer, 0, FILE_IMAGE_LENGTH);
	length = mysql_read(&mysql, buffer, FILE_IMAGE_LENGTH);

	write_image("a.jpg", buffer, length);


Exit:
	mysql_close(&mysql);

	return 0;
}




