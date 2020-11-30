#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

#define MYC_DB_SERVER_IP "192.168.249.133"
#define MYC_DB_SERVER_PORT 3306

#define MYC_DB_USERNAME "admin"
#define MYC_DB_PASSWORD "123456"

#define MYC_DB_DEFAULTDB "MYC_DB"

#define SQL_INSERT_TBL_USER "INSERT TBL_USER(U_NAME, U_GENGDER) VALUES('qiuxiang', 'woman');"


#define nullptr NULL

//C U R D --> creat, update, read, delete

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
	
	if(mysql_real_query(&mysql, SQL_INSERT_TBL_USER, strlen(SQL_INSERT_TBL_USER))){
		printf("ysql_real_query : %s\n", mysql_error(&mysql));
	}

	mysql_close(&mysql);
	printf("done\n");

	return 0;
}




