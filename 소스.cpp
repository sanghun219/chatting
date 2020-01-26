#include <iostream>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <sstream>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main()
{
	// init winsock
	WSADATA wsData;
	WORD word = MAKEWORD(2, 2);

	int wsOk = WSAStartup(word, &wsData);
	if (wsOk != 0)
	{
		cerr << "not Init WinSock" << endl;
		return 0;
	}

	// create sock

	SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listeningSocket == INVALID_SOCKET)
	{
		cerr << "can't create sock" << endl;
		return 0;
	}

	// bind sock ip address and port to a socket
	SOCKADDR_IN hint;
	hint.sin_family = AF_INET;
	//host to network short
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listeningSocket, (sockaddr*)&hint, sizeof(hint));

	// tell winsock the socket is for listening
	// SOMAXCONN : 네트워크 연결 최대 개수
	listen(listeningSocket, SOMAXCONN);

	fd_set master;
	FD_ZERO(&master);

	FD_SET(listeningSocket, &master);

	while (true)
	{
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listeningSocket)
			{
				//Accept a new connection
				SOCKET client = accept(listeningSocket, nullptr, nullptr);
				//Add a new connection to the list of connected clients

				FD_SET(client, &master);

				//send a welcome message to the connected client
				string wellcomeMsg = "Welcome to the Awesome Chat Server!\n";
				send(client, wellcomeMsg.c_str(), wellcomeMsg.size() + 1, 0);

				//TODO : BroadCast we have a new connection
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					//Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outsock = master.fd_array[i];
						if (outsock != listeningSocket && outsock != sock)
						{
							ostringstream ss;
							ss << "socket #" << sock << ":"
								<< buf << "\r\n";
							string strOut = ss.str();

							send(outsock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}

					//Send Message
				}

				//Accept a new message

				// Send Message to other clients, and definately NOT the listening socket
			}
		}
	}

	WSACleanup();

	return 0;
}

//// wait for a connection

	//SOCKADDR_IN client;
	//int clientSize = sizeof(client);

	//SOCKET clientSocket = accept(listeningSocket, (sockaddr*)&client, &clientSize);
	//if (clientSocket == INVALID_SOCKET)
	//{
	//	//...
	//}

	//char host[NI_MAXHOST]; // clients remote name
	//char service[NI_MAXSERV]; // Service (i.e. port) the Client is connect on

	//ZeroMemory(host, NI_MAXHOST);
	//ZeroMemory(service, NI_MAXSERV);

	//if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	//{
	//	cout << host << " connected on port" << service << endl;
	//}
	//else
	//{
	//	//ipv4,ipv6를 사람이 알아보기 쉬운 형태로 표현
	//	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
	//	cout << host << " connected on port" <<
	//		ntohs(client.sin_port) << endl;
	//}

	//// close listening socket

	//closesocket(listeningSocket);

	//// while loop : accept and echo message back to client

	//char buf[4096];
	//while (true)
	//{
	//	ZeroMemory(buf, 4096);

	//	// wait for client to send data

	//	int bytesReceived = recv(clientSocket, buf, 4096, 0);
	//	if (bytesReceived == SOCKET_ERROR)
	//	{
	//		cerr << "Error in recv(). Quitting" << endl;
	//		break;
	//	}

	//	if (bytesReceived == 0)
	//	{
	//		cout << "client disconnected" << endl;
	//		break;
	//	}
	//	// echo message back to client

	//	send(clientSocket, buf, bytesReceived + 1, 0);
	//}

	// close sock