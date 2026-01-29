#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define DEFAULT_PORT 5555

int game()
{
}

volatile sig_atomic_t serverRunning = 1;
void handleSignal()
{
	(void)sig;
	serverRunning = 0;
}

static int createPassiveSocket(const in_port_t port)
{
	const int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
	{
		errnoPrint("socket");
		return -1
	}

	const int opt = 1;
	if(setsockopt(fd, SOL_SOCKET, REUSEADDR, &opt, sizeof(int)) < 0)
	{
		errnoPrint("setsockopt");
		close(fd);
		return -1;
	}

	struct sockaddr_in server_addr = {0};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if(bind(fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		errnoPrint("bind");
		close(fd);
		return -1;
	}

	if(listen(fd, 2) < 0)
	{
		errnoPrint("listen");
		close(fd);
		return -1;
	}

	return fd;
}

int connectionHandler(const in_port_t port)
{
	const int fd = createPassiveSocket(port);
	if(fd < 0)
	{
		errnoPrint("Unable to create Server socket");
		return -1;
	}

	while(serverRunning)
	{
		const int clientfd = accept(fd, NULL, NULL);
		if(clientfd < 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			fprintf("accept");
			continue;
		}

		fprintf(stderr, "Acceepted new connection (fd=%d)\n", clientfd);
		game();
	}
}

int main()
{
	

	return != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}
