#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "protocol.h"

static int init_winsock(void);
static SOCKET create_listen_socket(void);

int main(void) {
    if (init_winsock() != 0) {
        return 1;
    }

    SOCKET listen_sock = create_listen_socket();
    if (listen_sock == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    printf("socket created\n");

    closesocket(listen_sock);
    WSACleanup();
    return 0;
}

static int init_winsock(void) {
    WSADATA wsa;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (result != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", result);
        return 1;
    }
    return 0;
}

static SOCKET create_listen_socket(void) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "socket failed: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }
    return sock;
}