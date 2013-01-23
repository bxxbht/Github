#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <pthread.h>

#define SHM_MSG		0x8888
#define SHM_SIZE 	1024
#define SHM_SIZE_DOUBLE SHM_SIZE*2

#if 0
static char *cyg_buf;
static char *cyg_buf_ping;
static char *cyg_buf_pong;
static int cyg_buf_index;
static int cyg_init = 1;
static pthread_mutex_t mutex;

/* func_trace.c */
__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
	/* printf("-> %p\n", this_fn); */

	if (cyg_init) return;

	pthread_mutex_lock(&mutex);

	cyg_buf_index += sprintf(&cyg_buf[cyg_buf_index], "-> %p\n", this_fn);
	if ((cyg_buf_index + 64) > SHM_SIZE) {
		sprintf(&cyg_buf[cyg_buf_index], "<---- next ---->\n");
		cyg_buf = (cyg_buf != cyg_buf_ping) ?  cyg_buf_ping : cyg_buf_pong;
		memset(cyg_buf, 0, SHM_SIZE);
		cyg_buf_index = 0;
	}

	pthread_mutex_unlock(&mutex);
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
	/* printf("<- %p\n", this_fn); */

	if (cyg_init) return;

	pthread_mutex_lock(&mutex);

	cyg_buf_index += sprintf(&cyg_buf[cyg_buf_index], "<- %p\n", this_fn);
	cyg_buf[cyg_buf_index] = 0;
	if ((cyg_buf_index + 64) > SHM_SIZE) {
		cyg_buf_index += sprintf(&cyg_buf[cyg_buf_index], "<---- next ---->\n");
		cyg_buf[cyg_buf_index] = 0;
		cyg_buf = (cyg_buf != cyg_buf_ping) ?  cyg_buf_ping : cyg_buf_pong;
		cyg_buf[0] = 0;
		cyg_buf_index = 0;
	}

	pthread_mutex_unlock(&mutex);
}

__attribute__((no_instrument_function))
void cyg_profile_init(void)
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

	cyg_buf_ping = shared_memory;
	cyg_buf_pong = &cyg_buf_ping[SHM_SIZE];
	cyg_buf = cyg_buf_ping;
	cyg_buf_index = 0;

	pthread_mutex_init(&mutex, NULL);

	cyg_init = 0;
}
#else

#define FUNC_ENTER 0x55555555
#define FUNC_EXIT  0xAAAAAAAA

struct cyg_func_t {
	int action;
	void *func;
};

static unsigned char *cyg_buf;
static int *cyg_buf_index;
static int cyg_init = 1;
static pthread_mutex_t mutex;

/* func_trace.c */
__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
	struct cyg_func_t type;
	/* printf("-> %p\n", this_fn); */

	if (cyg_init) return;

	/* pthread_mutex_lock(&mutex); */

	type.action = FUNC_ENTER;
	type.func = this_fn;

	memcpy(&cyg_buf[*cyg_buf_index], &type, sizeof(struct cyg_func_t));
	*cyg_buf_index += sizeof(struct cyg_func_t);

	if (*cyg_buf_index >= (SHM_SIZE_DOUBLE - 8)) {
		*cyg_buf_index = 0;
	}

	/* pthread_mutex_unlock(&mutex); */
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
	struct cyg_func_t type;
	int index;
	/* printf("<- %p\n", this_fn); */

	if (cyg_init) return;

	/* pthread_mutex_lock(&mutex); */
#if 1
	index = *cyg_buf_index - sizeof(struct cyg_func_t);
	if (index < 0) {
		index = SHM_SIZE_DOUBLE - 8 - sizeof(struct cyg_func_t);
	}
	memcpy(&type, &cyg_buf[index], sizeof(struct cyg_func_t));

	if ((type.action == FUNC_ENTER) && (type.func == this_fn)) {
		type.action = 0;
		*cyg_buf_index = index;
		return;
	}
#endif

	type.action = FUNC_EXIT;
	type.func = this_fn;

	memcpy(&cyg_buf[*cyg_buf_index], &type, sizeof(struct cyg_func_t));
	*cyg_buf_index += sizeof(struct cyg_func_t);

	if (*cyg_buf_index >= (SHM_SIZE_DOUBLE - 8)) {
		*cyg_buf_index = 0;
	}

	/* pthread_mutex_unlock(&mutex); */
}

__attribute__((no_instrument_function))
void cyg_profile_init(void)
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

	cyg_buf = (unsigned char *)shared_memory + 8;
	cyg_buf_index = (int *)shared_memory;
	*cyg_buf_index = 0;

	pthread_mutex_init(&mutex, NULL);

	cyg_init = 0;
}
#endif
