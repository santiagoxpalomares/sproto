#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT "9090"
#define BUF_SIZE 1024

static int send_all(SOCKET sock, const char *data, int len)
{
    int sent = 0;
    while (sent < len) {
        int s = send(sock, data + sent, len - sent, 0);
        if (s == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            return -1;
        }
        sent += s;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    const char *host = (argc > 1) ? argv[1] : DEFAULT_HOST;
    const char *port = (argc > 2) ? argv[2] : DEFAULT_PORT;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(host, port, &hints, &res) != 0) {
        printf("getaddrinfo failed\n");
        WSACleanup();
        return 1;
    }

    SOCKET sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        freeaddrinfo(res);
        WSACleanup();
        return 1;
    }

    if (connect(sock, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) {
        printf("connect failed: %d\n", WSAGetLastError());
        freeaddrinfo(res);
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(res);

    printf("Connected to %s:%s. Type a message and press Enter. Empty line quits.\n", host, port);

    char line[BUF_SIZE];
    char reply[BUF_SIZE];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        if (line[0] == '\n' || line[0] == '\0') {
            break;
        }

        if (send_all(sock, line, (int)strlen(line)) != 0) {
            break;
        }

        int n = recv(sock, reply, sizeof(reply) - 1, 0);
        if (n > 0) {
            reply[n] = '\0';
            printf("server: %s", reply);
        } else if (n == 0) {
            printf("server closed the connection\n");
            break;
        } else {
            printf("recv failed: %d\n", WSAGetLastError());
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}