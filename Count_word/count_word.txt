#include <stdio.h>
#define OUT		0
#define IN		1
#define init	OUT
int count_word (char* filename){
    int status = init;
    int word = 0;
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) return -1;
    char c;
    while((c = fgetc(fp)) != EOF){
        if (c > 'z' || c < 'a' ){
	status = OUT;
        }
        else if (OUT == status) {
	status = IN;
	word++;
        }
    }
    return word;
}

int main(int argc, char* argv[]) {
    if(argc < 2) return -1;
    printf("word: %d\n", count_word(argv[1]));
}








