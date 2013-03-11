
#include "dir_tra.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>

//遍历文件夹图片
void tranves_file_desk(char *desk[],int num) 
{

	int i = 0;
	for(;i < num;i++){
		fprintf(stdout,"%s\n",desk[i]);	
	}
}
//程序结束回收空间
void destroy_file_desk(char *desk[],int num)
{
	int i = 0;
	for(;i < num;i++)
		free(desk[i]);
}
//判断是否文件名是否全为digital
int is_all_digit(char *filename)
{
	int i = 0;
	while(filename[i] != '\0')
	{
		if(isdigit(filename[i]) != 1)	
			return 0;
		i++;
	}
	return 1;
}
//读文件夹里jpg文件
int read_jpg_dir(char *path,char *file_desk[],int *pic_num)
{	

	DIR *dir = opendir(path);
	if(dir == NULL){
		fprintf(stderr,"open dir failed,%s\n",strerror(errno));	
		return -1;	
	}

	struct dirent *files;
	*pic_num = 0;
	while((files = readdir(dir)) != NULL)
	{
		if(is_all_digit(files->d_name) == 1)	
			continue;
		if(strstr(files->d_name,".jpg") == NULL)
			continue;

		//fprintf(stdout,"%s\n",files->d_name);
		file_desk[*pic_num] = malloc(sizeof(char) * strlen(files->d_name) + 1);

		if(file_desk[*pic_num] == NULL){
			fprintf(stderr,"malloc for file_desk[%d] failed,%s\n",*pic_num,strerror(errno));	
			return -2;
		}
		memcpy(file_desk[*pic_num],files->d_name,strlen(files->d_name));
		file_desk[*pic_num][strlen(files->d_name)] = '\0';
		//fprintf(stdout,"file_desk[%d] %s\n",*pic_num,file_desk[*pic_num]);
		(*pic_num)++;
	}
	closedir(dir);
	return 0;
}
