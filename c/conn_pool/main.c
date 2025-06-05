#include <limits.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include "thread_pool.h"

void *worker(void *user_data)
{
	int val;
	thread_pool_t *pool = (thread_pool_t *)user_data;
	pthread_t t_id = pthread_self();

	if (!pool) {
		return NULL;
	}

	while (1) {
		// printf("thread: %lu -- waiting to get conn\n",
		//        (unsigned long)t_id);
		val = thread_pool_get_conn(pool);
		// printf("thread: %lu -- got fd: %d\n", (unsigned long)t_id, val);

		if (!thread_pool_get_should_run(pool)) {
			break;
		}

		usleep(500);

		// if (val != INT_MIN) {
		// 	printf("thread: %lu -- got fd: %d\n",
		// 	       (unsigned long)t_id, val);
		// }
	}

	// printf("thread: %lu -- finished\n", (unsigned long)t_id);

	return NULL;
}

int main(int argc, char **argv)
{
	int workers = atoi(argv[1]);
	printf("using %d workers\n", workers);
	thread_pool_t *pool = thread_pool_create(workers, worker);

	if (!pool) {
		goto out;
	}

	for (int i = 0; i < 10000; i++) {
		// printf("thread: main -- pushing fd %d\n", i);
		thread_pool_push_conn(pool, i);
	}

	// printf("thread: main -- cleaning up\n");

	while (thread_pool_get_size(pool) > 0) {
		usleep(1000);
	}

out:
	thread_pool_destroy(pool);
	printf("thread: main -- done\n");
	// int sock = 0;
	// int client_fd;
	// struct sockaddr_in addr;
	// thread_queue_t *tq = NULL;

	// tq = thread_queue_create();

	// sock = socket(AF_INET, SOCK_STREAM, 0);
	// if (sock <= 0) {
	// 	printf("failed to create socket\n");
	// 	return -1;
	// }

	// addr.sin_family = AF_INET;
	// addr.sin_port = htons(8080);
	// addr.sin_addr.s_addr = INADDR_ANY;

	// if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
	// 	printf("failed to bind to port 8080\n");
	// 	return -1;
	// }

	// if (listen(sock, 10) < 0) {
	// 	printf("failed to listen\n");
	// 	return -1;
	// }

	// while (1) {
	// 	client_fd = accept(sock, NULL, NULL);

	// 	if (client_fd > 0) {
	// 		thread_queue_enqueue(tq, client_fd);
	// 	}
	// }

	// out:
	// 	thread_queue_destroy(tq);

	return 0;
}