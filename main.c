#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// #define ERROR_LEFT_SIDE err = 
// #define ERROR_RIGHT_SIDE if (err){printf("An error occured: %d", err);return 1;}

#define PORT 3000

const char *response = "HTTP/1.1 200 OK\n"
"Date: Sun, 18 Oct 2009 08:56:53 GMT\n"
"Accept-Ranges: bytes\n"
"Content-Length: 44\n"
"Connection: close\n"
"Content-Type: text/html\n"
"\n"
"<html><body><h1>It works!</h1></body></html>\n";

char* addCharsToString(char *baseText, char *newText);

struct sockaddr_in serv_addr;
struct sockaddr_in client_addr;

struct params
{
	int clientSocket;
	char* fileName;
};

void *thread(void *params)
{
	struct params *userData = (struct params*) params;
	int clientSocket = userData->clientSocket;

	int newLines = 0;
	char characters[2];

	while(newLines < 2)
	{
		read(clientSocket, characters, 2);
		if (characters[0] == '\n' || characters[1] == '\n')
		{
			newLines++;
		}
		printf("%c%c", characters[0], characters[1]);
	}

	printf("%d\n", userData->clientSocket);

	write(clientSocket, response, strlen(response));

	close(clientSocket);
}

int main()
{
	int err;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 
    if (serverSocket < 0)
    {
        printf("ERROR opening socket");
    }

	if (bind(serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR on binding");
	}

	listen(serverSocket, 5);
	
	while(1)
	{
		int clientSize = sizeof(client_addr);
		int clientSocket = accept(serverSocket, (struct sockaddr *) &client_addr, &clientSize);

     	if (clientSocket < 0) 
     	{
        	printf("Error at accept! Line: %d!", __LINE__);
     	}

     	struct params userData;
     	userData.clientSocket = clientSocket;

		pthread_t clientThread;
		pthread_create(&clientThread, NULL, thread, &userData);
	}

	return 0;
}

char* addCharsToString(char *baseText, char *newText)
{
	int newSize = strlen(baseText) + strlen(newText);
	char *result = malloc(sizeof(char) * (strlen(baseText) + strlen(newText)));

	strcpy(result, baseText);
	strcat(result, newText);

	return result;
}