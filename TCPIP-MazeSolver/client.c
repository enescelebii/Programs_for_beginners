#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Winsock kütüphanesi bağlantısı

#define PORT 8081
#define SERVER_IP "127.0.0.1"

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serverAddr;
    char buffer[1024] = {0};

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock basarisiz. Hata kodu: %d\n", WSAGetLastError());
        return 1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket basarisiz. Hata kodu: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Baglanti basarisiz. Hata kodu: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Sunucuya baglandi. Hareket komutlari:\n");
    printf("w: Yukari, s: Asagi, a: Sol, d: Sag\n");

    while (1) {
        printf("Hareketinizi girin: ");
        char command;
        scanf(" %c", &command);

        send(sock, &command, 1, 0);

        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(sock, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            printf("Sunucudan baglanti kesildi.\n");
            break;
        }

        printf("Sunucu yaniti:\n%s\n", buffer);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
