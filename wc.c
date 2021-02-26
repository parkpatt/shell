#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include<string.h>

void wc(int mode, char* path){
	/*Feel free to change the templates as needed*/
	/*insert code here*/
	FILE *fp = fopen(path,"r");
	if(fp == NULL){
		printf("File does not exist \n");
		exit(1);
	}
	int cCount,wCount,lCount;
	cCount=0;
	wCount=0;
	lCount=0;
	char character;
	while((character = fgetc(fp))!= EOF){
		cCount++;

		if(character == ' ' || character == '\n' || character == '\0'){
			wCount++;
		}
		if(character == '\n'){
			lCount++;
		}

	}
	if(mode == 0){
		printf("c: %d w: %d l: %d\n",cCount,wCount,lCount);
	}
	else if(mode == 1){
		printf("l: %d\n",lCount);
	}
	else if(mode == 2){
		printf("w: %d\n",wCount);
	}
	else if(mode == 3){
		printf("c: %d\n",cCount);
	}
	else{
		printf("Error, invalid tag \n");
		exit(1);
	}
	fclose(fp);
}

int main(int argc, char** argv){
	if(argc>2){
		if(strcmp(argv[1], "-l") == 0) {
			wc(1, argv[2]);
		} else if(strcmp(argv[1], "-w") == 0) {
			wc(2, argv[2]);
		} else if(strcmp(argv[1], "-c") == 0) {
			wc(3, argv[2]);
		} else {
			printf("Invalid arguments\n");
		}
	} else if (argc==2){
	 	if(strcmp(argv[1], "-l") == 0) {
			wc(1, NULL);
		} else if(strcmp(argv[1], "-w") == 0) {
			wc(2, NULL);
		} else if(strcmp(argv[1], "-c") == 0) {
			wc(3, NULL);
		} else {
    		wc(0, argv[1]);
    	}
  	} else {
  		wc(0,NULL);
  	}

	return 0;
}
