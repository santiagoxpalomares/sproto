#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define PORT "9090"
#define BUF_SIZE 1024

static void handle_client(SOCKET client, const char *peer)
{
    char buf[BUF_SIZE];

    for (;;) {
        int n = recv(client, buf, sizeof(buf), 0);

        if (n > 0) {
            printf("[%s] received %d bytes\n", peer, n);

            int sent = 0;
            while (sent < n) {
                int s = send(client, buf + sent, n - sent, 0);
                if (s == SOCKET_ERROR) {
                    printf("[%s] send failed: %d\n", peer, WSAGetLastError());
                    return;
                }
                sent += s;
            }
        } else if (n == 0) {
            printf("[%s] disconnected\n", peer);
            return;
        } else {
            printf("[%s] recv failed: %d\n", peer, WSAGetLastError());
            return;
        }
    }
}

int main(void)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        printf("getaddrinfo failed\n");
        WSACleanup();
        return 1;
    }

    SOCKET listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listener == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        freeaddrinfo(res);
        WSACleanup();
        return 1;
    }

    if (bind(listener, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        freeaddrinfo(res);
        closesocket(listener);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(res);

    if (listen(listener, SOMAXCONN) == SOCKET_ERROR) {
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    printf("Listening on port %s...\n", PORT);

    for (;;) {
        struct sockaddr_in addr;
        int addrlen = sizeof(addr);

        SOCKET client = accept(listener, (struct sockaddr *)&addr, &addrlen);
        if (client == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            continue;
        }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
        char peer[64];
        snprintf(peer, sizeof(peer), "%s:%u", ip, ntohs(addr.sin_port));

        printf("[%s] connected\n", peer);
        handle_client(client, peer);
        closesocket(client);
    }

    closesocket(listener);
    WSACleanup();
    return 0;
}