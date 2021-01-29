#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996) 

#include<stdio.h>
#include<stdlib.h>
#include <winsock2.h>
#include <windows.h>
#define True 1
#define false 0

void main(int argc, char** argv) //entry point
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int ret;
	SOCKET sClient;
	SOCKADDR_IN saServer;

	BOOL fSuccess = True;

	if (argc <= 1)
	{
		printf("USAGE: TCPClient <Server IP address>.\n");
		getchar();
		return;
	}

	//WinSock Initiate
	wVersionRequested = MAKEWORD(2, 2);
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0)
	{
		printf("WSAStarup() failed！\n");
		return;
	}

	//create Socket, use TCP protocol
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == INVALID_SOCKET)
	{
		WSACleanup();
		printf("socket() failed!\n");
		return;
	}

	saServer.sin_family = AF_INET;
	saServer.sin_port = htons(5150);
	saServer.sin_addr.S_un.S_addr = inet_addr(argv[1]); ;

	//connect to server
	ret = connect(sClient, (struct sockaddr*)&saServer, sizeof(saServer));
	if (ret == SOCKET_ERROR)
	{
		printf("connect() failed!\n");
		closesocket(sClient);
		WSACleanup();
		return;
	}

	char sendMessage[] = "Hello, This is ME";
	ret = send(sClient, (char*)&sendMessage, sizeof(sendMessage), 0);
	if (ret == SOCKET_ERROR)
	{
		printf("send() failed!\n");
	}

	char recvBuf[1024];
	recv(sClient, recvBuf, 1024, 0);
	printf("%s\n", recvBuf);

	closesocket(sClient);
	WSACleanup();
	system("pause");
	return ;

}



