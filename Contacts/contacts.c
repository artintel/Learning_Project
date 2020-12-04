/*
 * @Author: Mayc
 * @Date: 2020-11-25 18:37:29
 * @LastEditTime: 2020-12-04 15:29:27
 * @FilePath: \VSC++&GO\ProjectLearning\Contacts\contacts.c
 * @这是一个漂亮的注释头
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PHONE 32
#define NAMELENGHT 16
#define nullptr NULL
#define BUFFER_LENGTH 128
#define MIN_TOKEN_LENGTH 5

#define INFO printf

//as for list:
//1. Insert
//2. Remove

#define LIST_INSERT(item, list) do {	\
	item->prev = nullptr;		  		\
	item->next = list;		      		\
	if((list)!=nullptr) (list)->prev = item;	\
	(list) = item;				  		\
} while(0)



#define List_REMOVE(item, list) do {	\
	if(item->prev != nullptr) item->prev->next = item->next;	\
	if(item->next != nullptr) item->next->prev = item->prev;	\
	if(list == item) list = item->next;							\
	item->prev = item->next = nullptr;							\
} while(0)

struct person{
	char name[NAMELENGHT];
	char phone[PHONE];

	struct person* next;
	struct person* prev;
};
struct contacts{
	struct person* people;
	int count;
};

enum {
	OPER_INSERT = 1,
	OPER_PRINT,
	OPER_DELETE,
	OPER_SEARCH,
	OPER_SAVE,
	OPER_LOAD
};

// define interface
int person_insert(struct person** ppeople, struct person *ps){
	if( ps == nullptr ) return -1;
	LIST_INSERT(ps, *ppeople);
	return 0;
}

int person_delete(struct person** ppeople, struct person *ps){
	if( ps == nullptr ) return -1;
	List_REMOVE(ps, *ppeople);
	return 0;
}

struct person* person_search(struct person* people, const char* name){

	struct person* item = nullptr;

	for(item = people; item != nullptr; item = item->next){
		if(!strcmp(name, item->name))
			break;
	}
	return item;
}

int person_traversal(struct person* people){

	struct person* item = nullptr;
	for(item = people; item != nullptr; item = item->next){
		INFO("name: %s, phone: %s\n", item->name, item->phone);
	}
	return 0;
}

int save_file(struct person* people, const char* filename){
	FILE* fp = fopen(filename, "w");
	if (fp == nullptr) return -1;
	struct person* item = nullptr;
	for( item = people; item != nullptr; item = item->next){
		fprintf(fp, "name: %s, phone: %s\n", item->name, item->phone);
		fflush(fp);
	}
	fclose(fp);
	return 0;
}

int parser_token(char* buffer, int length, char* name, char* phone){

	if(buffer == nullptr) return -1;
	if ( length < MIN_TOKEN_LENGTH ) return -2;
	
	int i = 0, j = 0, status = 0;
	
	for(i = 0; buffer[i] != ','; i++){
		if(buffer[i] == ' '){
			status = 1;
		}
		else if (status == 1){
			name[j++] = buffer[i];
		}
	}
	status = 0, j = 0;
	for(; i < length; i++){
		if(buffer[i] == ' '){
			status = 1;
		}
		else if (status == 1){
			phone[j++] = buffer[i];
		}
	}

	INFO("file token: %s --> %s\n", name, phone);
	return 0;
		
	
}

int load_file(struct person** ppeople,int* count, const char* filename){
	FILE* fp = fopen(filename, "r");
	if (fp == nullptr) return -1;
	while(!feof(fp)){
		char buffer[BUFFER_LENGTH] = { 0 };
		fgets(buffer, BUFFER_LENGTH, fp);
		int length = strlen(buffer);
		//INFO("length: %d\n", length);

		
		//name: qiuxiang, phone: 123456
		char name[NAMELENGHT] = { 0 };
		char phone[PHONE] = { 0 };
		if(0 != parser_token(buffer, strlen(buffer), name, phone)){
			continue;
		}

		struct person* p = (struct person*)malloc(sizeof(struct person));
		if(p == nullptr) return -2;
		memcpy(p->name, name, NAMELENGHT);
		memcpy(p->phone, phone, PHONE);

		person_insert(ppeople, p);
		(*count)++;
	}
	fclose(fp);
}


// end

int insert_entry(struct contacts* cts){
	if( cts == nullptr ) return -1;

	struct person* p = (struct person*)malloc(sizeof(struct person));
	if( p == nullptr ) return -2;
	
	// name 
	INFO("Please Input Name: \n");
	scanf("%s", p->name);
	
	// phone
	INFO("Please Input Phone: \n");
	scanf("%s", p->phone);
	// add people
	if( 0 != person_insert(&cts->people, p)){
		free(p);
		return -3;
	}
	cts->count++;
	INFO("Insert Success\n");
}

int print_entry(struct contacts* cts){ 
	if ( cts == nullptr ) return -1;
	person_traversal(cts->people);

	// cts->people
}

int delete_entry(struct contacts* cts){
	if( cts == nullptr ) return -1;
	INFO("Please Input Name: \n");
	// name
	char* name;
	scanf("%s", name);

	
	// person
	struct person* ps = person_search(cts->people, name);
	if( ps == nullptr ) {
		INFO("Person don't Exit\n");
		return -2;
	}

	// delete
	person_delete(&cts->people, ps);
	free(ps);
	return 0;
}

int search_entry(struct contacts* cts){
	// name
	// person
	if( cts == nullptr ) return -1;
	INFO("Please Input Name: \n");
	// name
	char* name;
	scanf("%s", name);

	
	// person
	struct person* ps = person_search(cts->people, name);
	if( ps == nullptr ) {
		INFO("Person don't Exit\n");
		return -2;
	}
	INFO("name: %s, phone: %s\n", ps->name, ps->phone);
	return 0;
}

int save_entry(struct contacts* cts){
	if(cts == nullptr) return -1;
	INFO("Please Input Save Filename: \n");
	char filename[NAMELENGHT] = { 0 };
	scanf("%s", filename);
	save_file(cts->people, filename);
	return 0;
}

int load_entry(struct contacts* cts){
	if (cts == nullptr) return -1;
	INFO("Please Input Load Filename: \n");
	char filename[NAMELENGHT] = { 0 };
	scanf("%s", filename);
	load_file(&cts->people, &cts->count, filename);
	return 0;
}

void menu_info(void){
	INFO("\n\n****************************************************************************************************\n\n");
	INFO("********************\t\t 1. Add Person\t\t2. Print People\t\t********************\n");
	INFO("********************\t\t 3. Del Person\t\t4. Search People\t********************\n");
	INFO("********************\t\t 5. Save Person\t\t6. Load People\t\t********************\n");
	INFO("********************\t\t \tOther Key for Exiting Program\t\t********************\n");
	INFO("\n****************************************************************************************************\n\n");
}

int main(){
	menu_info();

	//struct contacts cts = { 0 };
	struct contacts* cts = (struct contacts*)malloc(sizeof(struct contacts));
	if( cts == nullptr ) return -1;
	memset(cts, 0, sizeof(struct contacts));
	while(1){
		int select = 0;
		scanf("%d", &select);
		switch ( select ){
			case OPER_INSERT:
				//insert_entry(&cts);
				insert_entry(cts);
				break;
			case OPER_PRINT:
				print_entry(cts);
				break;
			case OPER_DELETE:
				delete_entry(cts);
				break;
			case OPER_SEARCH:
				search_entry(cts);
				break;
			case OPER_SAVE:
				save_entry(cts);
				break;
			case OPER_LOAD:
				load_entry(cts);
				break;
			default:
				goto exit;
		}
	}
exit:
	free(cts);
	return 0;
}
