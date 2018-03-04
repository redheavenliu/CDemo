#include <dirent.h>
#include "m_stdio.h"

static void
main_1
(
    int argc,
    char **argv
)
{
	DIR *dp;
	struct dirent *dirp;

	if((dp = opendir(argv[0])) != NULL){
		while((dirp = readdir(dp)) != NULL){
			printf("%s\n",dirp->d_name);
		}
	}else{
		log_e("opendir fail.");
	}
}


int main(int argc,char *argv[])
{
	if(argc >= 2){
		int num = atoi(argv[1]);
		switch(num){
		case 1:
			main_1(argc,&argv[2]);
			break;
		case 2:

			break;
		default:
			log_e("No such method.");
			return 1;
		}
		return 0;
	}else{
		return 1;
	}

}


