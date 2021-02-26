#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

void ls_recurse(char *path)
{
	printf("path: %s\n", path);
	struct dirent **nameList; //initiate a local pointer for use with scandir call
	int dircount;//the number of entries returned by scandir call
	dircount = scandir(path, &nameList, NULL, alphasort);
	if(dircount == -1){
		return;
	}
	printf("number of entries: %d\n", dircount-2);
	for (int i = 2; i<dircount; i++) // it's starting at 2 because the first 2 are self and parent
	{
		printf("   %s\n",nameList[i]->d_name);
	}
	for (int i = 2; i<dircount; i++)
	{
		if (nameList[i]->d_type == DT_DIR)
		{
			char newpath[1024];
			strcpy(newpath, path);
			char slash[2] = "/";
			strcat(newpath, slash);
			strcat(newpath, nameList[i]->d_name);
			ls_recurse(newpath);
		}
	}
	for (int i = 0; i<dircount; i++)
	{
		free(nameList[i]);
	}
	free(nameList);
}

void ls(char *path, bool recurse_flag)
{

	struct dirent **nameList; //initiate a local pointer for use with scandir call
	int dircount;//the number of entries returned by scandir call

	if (path == NULL) //this means use cwd
	{
		char cwd_path[1024]; //place to put cwd
		getcwd(cwd_path, sizeof(cwd_path)); //get the cwd
		if (cwd_path == NULL) //error check
		{
			return;
		}
		path = cwd_path; //set path to cwd
	}
	printf("path: %s\n", path);
	if (recurse_flag == false)
	{ //No need to recurse just print the contents
		dircount = scandir(path, &nameList, NULL, alphasort);//need to free namelist at some point
		if(dircount == -1){
			return;
		}
		printf("number of entries: %d\n", dircount-2);
		for (int i = 2; i<dircount; i++)
		{
			printf("   %s\n",nameList[i]->d_name);
		}
		for (int i = 0; i<dircount; i++)
		{
			free(nameList[i]);
		}
		free(nameList);
	}

	if (recurse_flag == true)
	{//need to recurse with helper function here and get new dirp and namelist
		dircount = scandir(path, &nameList, NULL, alphasort);
		if(dircount == -1){
			return;
		}
		printf("number of entries: %d\n", dircount-2);
		//start by printing the top directory contents like before
		for (int i = 2; i<dircount; i++) // it's starting at 2 because the first 2 are self and parent
		{
			printf("   %s\n",nameList[i]->d_name);
		}
		//but after the initial print, loop through again and call helper function
		//on any entry types that match DT_DIR
		for (int i = 2; i<dircount; i++)
		{
			if (nameList[i]->d_type == DT_DIR)
			{
				char newpath[1024];
				strcpy(newpath, path);
				char slash[2] = "/";
				strcat(newpath, slash);
				strcat(newpath, nameList[i]->d_name);
				ls_recurse(newpath);
			}
		}
		for (int i = 0; i<dircount; i++)
		{
			free(nameList[i]);
		}
		free(nameList);
	}
}




int main(int argc, char *argv[]){
	if(argc < 2){ // No -R flag and no path name
		ls(NULL, false);
	} else if(strcmp(argv[1], "-R") == 0) {
		if(argc == 2) { // only -R flag
			ls(NULL, true);
		} else { // -R flag with some path name
			ls(argv[2], true);
		}
	}else { // no -R flag but path name is given
    	ls(argv[1], false);
  }
	return 0;
}
