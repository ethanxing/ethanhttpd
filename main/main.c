#include <stdio.h>
#include <stdlib.h>

int main()
{
	int work_num = 4;
	pid_t pid;
	if()
	while(work_num--){
		if((pid = fork()) < 0){
			fprintf(stderr, "fork error!\n");
		}else if(pid == 0){
			do_work();
		}

	}
}
