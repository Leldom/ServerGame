#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "game.h"

extern volatile sig_atomic_t serverRunning;

static int secretNumber = 42;
static int winnerFound = 0;
static pthread_mutex_t gameMutex = PTHREAD_MUTEX_INITIALIZER;

void *game_thread(void *arg)
{
    int clientfd = *(int*)arg;
    free(arg);

    char buffer[256];
    const char* welcome = "Welcome to the number guessing game! Guess a number: \n";
    write(clientfd, welcome, strlen(welcome));

    while(serverRunning)
    {
        pthread_mutex_lock(&gameMutex);
        if(winnerFound)
        {
            const char* already_won = "Game Over! Someone has won!\n";
            write(clientfd, already_won, strlen(already_won));
            pthread_mutex_unlock(&gameMutex);
            break;
        }
        pthread_mutex_unlock(&gameMutex);

        ssize_t n = read(clientfd, buffer, sizeof(buffer) -1);
        if(n <= 0) break;

        buffer[n] = '\0';
        int guess = atoi(buffer);

        pthread_mutex_lock(&gameMutex);

        if(winnerFound)
        {
            const char* tooLate = "Too late, someone else was faster!\n";
            write(clientfd, tooLate, strlen(tooLate));
            pthread_mutex_unlock(&gameMutex);
            break;
        }

        if(guess == secretNumber)
        {
            winnerFound = 1;
            const char *winnerFoundMsg = "You have WON! Congratulations!\n";
            write(clientfd, winnerFoundMsg, strlen(winnerFoundMsg));
            pthread_mutex_unlock(&gameMutex);
            break;
        }else
        {
            const char* hint = (guess < secretNumber) ? "Too low!\n" : "Too high!\n";
            write(clientfd, hint, strlen(hint));
        }
        pthread_mutex_unlock(&gameMutex);
    }
    close(clientfd);
    return NULL;
}