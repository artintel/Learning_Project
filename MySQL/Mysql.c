#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

#define MYC_DB_SERVER_IP "192.168.249.133"
#define MYC_DB_SERVER_PORT 3306

#define MYC_DB_USERNAME "admin"
#define MYC_DB_PASSWORD "123456"

#define MYC_DB_DEFAULTDB "MYC_DB"

#define SQL_INSERT_TBL_USER "INSERT TBL_USER(U_NAME, U_GENGDER) VALUES('qiuxiang', 'woman');"
#define SQL_SELECT_TBL_USER "SELECT * FROM TBL_USER;"


#define SQL_DELETE_TBL_USER "CALL PROC_DELETE_USER('qiuxiang')"

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




int main(){
	MYSQL mysql;
	if(nullptr == mysql_init(&mysql)){
		printf("mysql_init : %s\n", mysql_error(&mysql));
		return -1;
	}

	if(!mysql_real_connect(&mysql, MYC_DB_SERVER_IP, MYC_DB_USERNAME, MYC_DB_PASSWORD, 
		MYC_DB_DEFAULTDB, MYC_DB_SERVER_PORT, nullptr, 0)){
		printf("ysql_read_connect : %s\n", mysql_error(&mysql));
	}

	// mysql --> insert
	printf("case: mysql --> insert \n");
#if 1	
	if(mysql_real_query(&mysql, SQL_INSERT_TBL_USER, strlen(SQL_INSERT_TBL_USER))){
		printf("ysql_real_query : %s\n", mysql_error(&mysql));
	}
#endif

	myc_mysql_select(&mysql);

	//MYSQL --> DELETE

	printf("case: mysql --> delete \n");


#if 1	
	if(mysql_real_query(&mysql, SQL_DELETE_TBL_USER, strlen(SQL_DELETE_TBL_USER))){
		printf("ysql_real_query : %s\n", mysql_error(&mysql));
	}
#endif

	myc_mysql_select(&mysql);



	mysql_close(&mysql);

	return 0;
}




