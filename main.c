#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "connection.h"

#define DEFAULT_PORT 5555

volatile sig_atomic_t serverRunning = 1;

void handleSignal(int sig)
{
	(void)sig;
	serverRunning = 0;
}

int main(int argc, char *argv[])
{
	struct sigaction sa = {0};
	sa.sa_handler = handleSignal;
	sigaction(SIGINT, &sa, NULL);

	if(argc > 2)
	{
		fprintf(stderr, "too much arguments\n");
		return EXIT_FAILURE;
	}

	in_port_t port = DEFAULT_PORT;
	if(argc > 1)
	{
		port = atoi(argv[1]);
	}

	fprintf(stderr, "Server started on port %d\n", port);
	int result = connectionHandler(port);

	if(result == -1)
	{
		return EXIT_FAILURE;
	}

	fprintf(stderr, "\nServer shutting down\n");
	return EXIT_SUCCESS;
}
