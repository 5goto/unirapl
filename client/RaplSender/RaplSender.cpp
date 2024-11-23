#include "RaplSender.h"


RaplSender* RaplSender::instance = nullptr;

RaplSender::RaplSender() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Ошибка при создании сокета" << std::endl;
    }

    // Заполнение адреса сервера
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) { // Замените на адрес вашего сервера
        std::cerr << "Ошибка преобразования IP-адреса" << std::endl;
    }

    serverAddress.sin_port = htons(8000);

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Ошибка при подключении к серверу" << std::endl;
    }
}

std::string RaplSender::request(std::string data) {
    send(clientSocket, data.c_str(), data.length(), 0);

    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        return std::string(buffer, bytesReceived);
    }
}

RaplSender::~RaplSender() {
    close(clientSocket);
}
