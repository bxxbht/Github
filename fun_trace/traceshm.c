/*
 * =====================================================================================
 *
 *       Filename:  traceshm.c
 *
 *    Description:  
 *
 *
 *        Version:  1.0
 *        Created:  08/18/2011 11:15:31 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SHM_MSG		0x8888
#define SHM_SIZE 	1024
#define SHM_SIZE_DOUBLE SHM_SIZE*2

static char buf_shadow[SHM_SIZE_DOUBLE];

int
main(int argc, char *argv[])
{
	void *shared_memory = (void *)0;
	int shmid;

	srand((unsigned int)getpid());

	shmid = shmget((key_t)SHM_MSG, SHM_SIZE_DOUBLE, 0666 | IPC_CREAT);
	if (shmid == -1) {
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	shared_memory = shmat(shmid, (void *)0, 0);
	if (shared_memory == (void *)-1) {
    		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	if (argc == 2 && (strcmp(argv[1], "init") == 0)) {
		printf("Trace shm Memory attached at %X\n", (int)shared_memory);
		memset(shared_memory, 0, SHM_SIZE_DOUBLE);

		if (0) {
			char *p = shared_memory;
			strcpy(p, "hello world\n");

			strcpy(&p[100], "xlongfeng\n");
		}

		while (1) sleep (1);
	} else {
#if 0
		int i;
		char *p = shared_memory;
		memcpy(buf_shadow, p, SHM_SIZE_DOUBLE);

		p = buf_shadow;

		printf("<-----------ping------------->\n");
		for (i = 0; i < SHM_SIZE; i++) {
			if (p[i]) putchar(p[i]);
			else break;
		}
		printf("<-----------pong------------->\n");
		p = &buf_shadow[SHM_SIZE];
		for (i = 0; i < SHM_SIZE; i++) {
			if (p[i]) putchar(p[i]);
			else break;
		}
		printf("<-----------end-------------->\n");

		if (shmdt(shared_memory) == -1) {
			fprintf(stderr, "shmdt failed\n");
			return -1;
		}
#else

#define FUNC_ENTER 0x55555555
#define FUNC_EXIT  0xAAAAAAAA
		struct cyg_func_t {
			int action;
			void *func;
		};

		struct cyg_func_t type;
		int *pindex = shared_memory;
		int index = *pindex;
		unsigned char *buf;
		int i;

		FILE *fp;

		memcpy(buf_shadow, (shared_memory + 8), SHM_SIZE_DOUBLE - 8);
		buf = buf_shadow;

		fp = fopen("/tmp/traceshm.txt", "w");

		if (!fp) {
			perror("fopen /tmp/traceshm.txt");
		}

		printf("<---------call stack trace--------->\n");

		for (i = index; i < SHM_SIZE_DOUBLE - 8; i+=sizeof(struct cyg_func_t)) {
			memcpy(&type, &buf[i], sizeof(struct cyg_func_t));
			if (type.action == FUNC_ENTER) {
				printf("%s -- %p\n", "Enter", type.func);
				fprintf(fp, "%s -- %p\n", "Enter", type.func);
			} else if (type.action == FUNC_EXIT) {
				printf("%s -- %p\n", "Exit ", type.func);
				fprintf(fp, "%s -- %p\n", "Exit ", type.func);
			}
		}

		for (i = 0; i < index; i+=sizeof(struct cyg_func_t)) {
			memcpy(&type, &buf[i], sizeof(struct cyg_func_t));
			if (type.action == FUNC_ENTER) {
				printf("%s -- %p\n", "Enter", type.func);
				fprintf(fp, "%s -- %p\n", "Enter", type.func);
			} else if (type.action == FUNC_EXIT) {
				printf("%s -- %p\n", "Exit ", type.func);
				fprintf(fp, "%s -- %p\n", "Exit ", type.func);
			}

		}
		printf("<---------------------------------->\n");

		fclose(fp);
#endif
	}

	return 0;
}
