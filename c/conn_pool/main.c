#include "thread_pool.h"
#include <limits.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void *worker(void *user_data)
{
	int val;
	thread_pool_t *pool = (thread_pool_t *)user_data;
	pthread_t t_id = pthread_self();

	if (!pool) {
		return NULL;
	}

	while (1) {
		val = thread_pool_get_conn(pool);

		if (!thread_pool_get_should_run(pool)) {
			break;
		}

		if (val != INT_MIN) {
			printf("hello from thread %lu -- val is: %d\n",
			       (unsigned long)t_id, val);
		}
	}

	printf("finished thread %lu\n", (unsigned long)t_id);

	return NULL;
}

int main(void)
{
	thread_pool_t *pool = thread_pool_create(3, worker);

	if (!pool) {
		goto out;
	}

	// printf("main: sleeping for 40\n");

	for (int i = 0; i < 27; i++) {
		printf("main: pushing %d\n", i);
		thread_pool_push_conn(pool, i);
	}

	while (pool->queue->list->size > 0) {
		sleep(5);
	}
	printf("main: cleaning up\n");
	// thread_pool_set_should_run(pool, 0);

out:
	thread_pool_destroy(pool);
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