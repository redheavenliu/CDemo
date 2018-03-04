#include "m_stdio.h"
#include "m_http.h"
#include "x_pic.h"

#define DEBUG 0
#define DEBUG_TIPS 1

//#define PATH "E:\\redheaven\\Linux_FileSystem\\data\\"
#define PATH "/mnt/hgfs/Linux_FileSystem/data/"


bool
x_pic_down
(
	x_pic_url_path_s *pic
)
{
    if(http_file_get(pic->url,pic->path) == 0){
		if(DEBUG || DEBUG_TIPS)
        	printf("Success:%s\n",pic->path);
		return TRUE;
    }else{
    	if(DEBUG || DEBUG_TIPS)
        	printf("Fail:%s\n",pic->path);
		return FALSE;
    }
}

void
x_pic_down_by_file
(
	const char* file_name
)
{
	FILE *fp = fopen(file_name,"r");
	if(fp == NULL){
		if(DEBUG)
			log_str("fopen fail.");
		goto exit_fail;
	}

	char buf[BUFFER_SIZE];
	while(fgets(buf,BUFFER_SIZE,fp) != NULL){
		if(DEBUG)
			printf("%s",buf);
		char *ptr = buf + strlen(buf) -1;
		while(*ptr){
			if(*ptr == '\r' || *ptr == '\n'){
				*ptr = '\0';
			}else{
				break;
			}
			ptr--;
		}

		int pos = str_indexof(buf,",");
		if(pos <= 0){
			if(DEBUG)
				printf("Fail for this line:%s",buf);
			goto exit_fail;
		}

		x_pic_url_path_s pic_s;
		pic_s.url = buf;
		pic_s.url[pos] = '\0';
		pic_s.path = buf + pos + 1;

		x_pic_down(&pic_s);

	}
	goto exit_success;
exit_fail:
	if(fp != NULL){
		fclose(fp);
	}
	if(DEBUG || DEBUG_TIPS)
		log_str("Complete:Fail.");
	return;
exit_success:
	if(fp != NULL){
		fclose(fp);
	}
	if(DEBUG || DEBUG_TIPS)
		log_str("Complete.");
	return;
}

void x_pic_main
(
    void
)
{
	//x_pic_down_by_file("E:\\redheaven\\Linux_FileSystem\\data\\pic.txt");
	x_pic_down_by_file("/mnt/hgfs/Linux_FileSystem/data/pic.txt");
}

