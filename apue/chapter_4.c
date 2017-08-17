#include "m_stdio.h"
#include "m_file.h"

static void
demo_4_1
(
    void
)
{
    printf("demo_4_1 start.\n");
    char *files[] = {"/etc/passwd","/etc","/dev/log","/dev/tty","/var/lib/oprofile/opd_pipe","/dev/sr0","/dev/cdrom",NULL};
    struct stat buf_stat;
    int i = 0;
    while(files[i] != NULL && lstat(files[i],&buf_stat) >= 0){
        if(S_ISREG(buf_stat.st_mode)){
            printf("%s: Regular file.\n",files[i]);
        }else if(S_ISDIR(buf_stat.st_mode)){
            printf("%s: Directory.\n",files[i]);
        }else if(S_ISCHR(buf_stat.st_mode)){
            printf("%s: Character file.\n",files[i]);
        }else if(S_ISBLK(buf_stat.st_mode)){
            printf("%s: Block file.\n",files[i]);
        }else if(S_ISFIFO(buf_stat.st_mode)){
            printf("%s: FIFO file.\n",files[i]);
        }else if(S_ISLNK(buf_stat.st_mode)){
            printf("%s: Symbolic file.\n",files[i]);
        }else if(S_ISSOCK(buf_stat.st_mode)){
            printf("%s: Socket file.\n",files[i]);
        }else{
            printf("%s: Unknown file.\n",files[i]);
        }

        i++;
    }

}

static void
demo_4_2
(
	void
)
{

}


int
main
(
    int argc,
    char **argv
)
{
    demo_4_1();
    return 0;
}

