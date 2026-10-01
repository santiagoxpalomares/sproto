#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <unistd.h>

#define PORT 9090

static int init_winsock(void);
static SOCKET create_listen_socket(void);

int main(void){
    // verify connection
    if (init_winsock() != 0 ){
        return 1
    }

    SOCKET listen_sock = create_listen_socket();
    if (listen_sock == INVALID_SOCKET){
        WSACleanup();
        return 1;
    }

    printf ("socket create\n now listening\n");

    closesocket(listen_sock);
    WSACleanup();
    return 0;
}

static int init_winsock(void){
    // TODO
}

static SOCKET create_listen_socket(void){
    // TODO
}