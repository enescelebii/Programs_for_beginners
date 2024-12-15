#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Winsock kütüphanesi bağlantısı

#define PORT 8081
#define MAX_CLIENTS 5

// 10x10'luk Labirent (Daha Estetik)
char maze[10][10] = {
    {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
    {'#', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', '#'},
    {'#', '#', ' ', '#', ' ', '#', ' ', '#', ' ', '#'},
    {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
    {'#', ' ', '#', '#', '#', ' ', '#', '#', ' ', '#'},
    {'#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
    {'#', ' ', '#', '#', ' ', '#', ' ', '#', ' ', '#'},
    {'#', ' ', '#', ' ', '#', ' ', ' ', ' ', ' ', '#'},
    {'#', 'P', '#', ' ', ' ', ' ', ' ', ' ', 'E', '#'},
    {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
};

// Oyuncunun başlangıç pozisyonu
typedef struct {
    int x, y;
} Player;

Player player = {8, 1}; 

void printMaze(char *buffer) {
    int index = 0;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            
            if (maze[i][j] == 'P') {
                buffer[index++] = 'P'; 
            } else if (maze[i][j] == 'E') {
                buffer[index++] = 'E'; 
            } else {
                buffer[index++] = maze[i][j]; 
            }
            buffer[index++] = ' '; 
        }
        buffer[index++] = '\n'; 
    }
    buffer[index] = '\0'; 
}

// Oyuncu hareket fonksiyonu
int movePlayer(char direction) {
    int newX = player.x, newY = player.y;

    if (direction == 'w') newX--;
    else if (direction == 's') newX++;
    else if (direction == 'a') newY--;
    else if (direction == 'd') newY++;

    if (maze[newX][newY] == ' ') {
        maze[player.x][player.y] = ' '; // Eski pozisyondaki oyuncuyu boşaltıyoruz
        maze[newX][newY] = 'P'; // Yeni pozisyonda oyuncuyu yerleştiriyoruz
        player.x = newX;
        player.y = newY;
        return 1; // Geçerli hareket
    } else if (maze[newX][newY] == 'E') {
        return 2; // Çıkışa ulaştı
    }
    return 0; // Geçersiz hareket
}

// Client ile iletişimi yöneten thread fonksiyonu
DWORD WINAPI handleClient(void *socket) {
    SOCKET clientSocket = *(SOCKET *)socket;
    free(socket);

    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            printf("Client disconnected.\n");
            closesocket(clientSocket);
            return 0;
        }

        char command = buffer[0];
        int moveResult = movePlayer(command);

        if (moveResult == 2) {
            char *winMessage = "Kazandiniz! Cikisa ulastiniz!\n";
            send(clientSocket, winMessage, strlen(winMessage), 0);
            closesocket(clientSocket);
            return 0;
        }

        // Labirenti gönderecek olan buffer
        char mazeState[128];
        printMaze(mazeState);
        send(clientSocket, mazeState, strlen(mazeState), 0);
    }
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock baslamadi. Hata kodu: %d\n", WSAGetLastError());
        return 1;
    }

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket olusturulamadi. Hata kodu: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind basarisiz. Hata kodu: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Listen basarisiz. Hata kodu: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Sunucu %d portunda calisiyor...\n", PORT);

    while (1) {
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize)) == INVALID_SOCKET) {
            printf("Accept basarisiz. Hata kodu: %d\n", WSAGetLastError());
            continue;
        }
        printf("Yeni bir istemci baglandi.\n");

        SOCKET *socketPtr = malloc(sizeof(SOCKET));
        *socketPtr = clientSocket;

        CreateThread(NULL, 0, handleClient, socketPtr, 0, NULL);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
