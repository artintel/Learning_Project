#include <stdio.h>
#include <string.h>
#define OUT		0
#define IN		1
#define init	OUT
int count_word (char* filename){
	int status = init;
	int word = 0;
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) return -1;
	char c;
	char str1[100][20];
	char str_temp[100];
	int frequency_word[100]= { 0 };
	int i;
	int num = 0;
	int index = 0;
	while((c = fgetc(fp)) != EOF){

		if ( c > 'z' || ('Z' < c && c < 'a') || c < 'A' ){
			if( c == '-' && status == IN){
				continue;
			}
			if( c == '\n' && status == IN) {
				continue;
			}
			if(status == IN){
				for(i = 0; i < num; i++){
					if(strcmp(str1[i], str_temp) == 0){
						frequency_word[i] += 1;
						break;
					}
				}
				if(i == num){
					strcpy(str1[i], str_temp);
					memset(str_temp, 0x00, sizeof(char)*100);
					frequency_word[i] += 1;
					num++;
				}
				index = 0;
				status = OUT;
			}
		}
		else if (OUT == status) {
			memset(str_temp, 0x00,sizeof(char)*100);
			str_temp[index++] = c;
			status = IN;
			word++;
		}
		else if (IN == status){
			str_temp[index++] = c;
		}
		
	}
	printf("\n");
	for(i = 0; i < num; i++){
		printf("word %-20s have %-20d times in this .txt\n", str1[i], frequency_word[i]);
	}
	return word;
}

int main(int argc, char* argv[]) {
	if(argc < 2) return -1;
	printf("word: %d\n", count_word(argv[1]));
}