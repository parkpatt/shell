
#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>

void cd(char* arg){
	/*insert code here*/
	char buf[150];
	if(chdir(arg) < 0){ //checks if it is a valid change in directory
		printf("Directory not found\n");
		exit(chdir(arg));
	}
	else{ //changes directory and prints results to the screen
		printf("%s\n", getcwd(buf,sizeof(buf)));
	}
}

int main(int argc, char** argv){

	if(argc<2){
		printf("Pass the path as an argument\n");
		return 0;
	}
	cd(argv[1]);
	return 0;
}
