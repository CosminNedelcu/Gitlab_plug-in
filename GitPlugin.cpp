#include <winsock2.h>   // addrinfo, WSACleanup 
#include <WS2tcpip.h>   // ai_family, ai_socktype, ai_protocop
#include <windows.h>    // include lib winsock2 and winsock - the libraries where we find the methods
#include <iostream>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main(void) {

    WSADATA wsaData;

    //Contains more info about the socket implementation
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed.\n";
        system("pause");
        return -1;
    }

    //Struct INFO will receive the network informations
    struct addrinfo info;

    ZeroMemory(&info, sizeof(info));
    info.ai_family = AF_INET;          //Protocol type
    info.ai_socktype = SOCK_STREAM;    //Connexion between two parties based on protocol
    info.ai_protocol = IPPROTO_TCP;    // TCP

    //Result will receive the Server informations
    struct addrinfo* result = NULL;

    //We take over the host address
    int iResult = getaddrinfo("www.github.com", NULL, &info, &result);
    if (iResult != 0 || result == NULL)
    {
        cout << "Could not resolve the Host Name" << endl;
        system("pause");
        WSACleanup();
        return 11;
    }

    //SOCKADDR_IN - is a specific transport structure for address and port - we take the informations from the server
    SOCKADDR_IN tempSock;
    tempSock.sin_addr = ((struct sockaddr_in*) result->ai_addr)->sin_addr;    //We take the ip address from Server
    tempSock.sin_family = AF_INET;  // IPv4
    tempSock.sin_port = htons(80);  // HTTP Port: 80

    //Free Result
    freeaddrinfo(result);

    //We create a socket for the communication with the Server - is a connection socket between this entities
    SOCKET commSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (commSocket == INVALID_SOCKET)
    {
        std::cout << "Creation of the Socket Failed" << std::endl;
        system("pause");
        WSACleanup();
        return -1;
    }

    //We are trying to connect to the server
    iResult = connect(commSocket, (SOCKADDR*)&tempSock, sizeof(tempSock));

    if (iResult != 0) {
        std::cout << "Error!!!";
        closesocket(commSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected.\n";

    //SendInfo contains the message
    const char* sendInfo = "GET / HTTP/1.1\r\n     api.github.com/users/NMihai10/repos      \r\nConnection: close\r\n\r\n";

    //Send the message to the server
    int iSendBytes = send(commSocket, sendInfo, strlen(sendInfo), 0);

    //We verify if the message could be sent and if the message is empty
    if (iSendBytes == SOCKET_ERROR || iSendBytes < strlen(sendInfo)) {
        std::cout << "Send failed!";
        closesocket(commSocket);
        WSACleanup();
        return 1;
    }


    //We receive the message from the Server
    char recvBuffer[10000];
    ZeroMemory(recvBuffer, sizeof(recvBuffer));

    //The length of the received message
    int recvLen;

    while ((recvLen = recv(commSocket, recvBuffer, sizeof(recvBuffer), 0) > 0))
    {
        int i = 0;
        while (recvBuffer[i] >= 32 || recvBuffer[i] == '\n' || recvBuffer[i] == '\r') {
            cout << recvBuffer[i];
            i += 1;
        }
    }

    //Close the connexion
    closesocket(commSocket);
    WSACleanup();

    system("pause");
    return 0;
}