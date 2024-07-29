#include "RaplDeamon.h"
#include <unistd.h>
#include "../RaplCore/Rapl.h"
#include "../utils/vec_str_converter.h"


RaplDeamon::~RaplDeamon() {
    close(clientSocket);
    close(serverSocket);
}

void RaplDeamon::run(int port) {
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressSize;

    logger.log("Starting unirapl...", Level::INFO);

    // Создание сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        logger.log("Socket creation error", Level::ERROR);
        logger.log("Stopping Unirapl with an error", Level::ERROR);
        return;
    }
    logger.log("Socket created successfully", Level::INFO);

    // Заполнение адреса сервера
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // Привязка сокета к адресу
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        logger.log("Socket binding error", Level::ERROR);
        logger.log("Stopping unirapl with an error", Level::ERROR);
        return;
    }
    logger.log("Socket is successfully bound to the address", Level::INFO);

    // Слушаем входящие подключения
    if (listen(serverSocket, 5) == -1) {
        logger.log("Socket listen error", Level::ERROR);
        logger.log("Stopping unirapl with an error", Level::ERROR);
        return;
    }
    logger.log("Server started at port [" + std::to_string(port) + "]", Level::INFO);

    // Цикл обработки подключений
    while (true) {
        clientAddressSize = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
        if (clientSocket == -1) {
            logger.log("Error accepting connection", Level::ERROR);
            continue;
        }

        std::string request;
        std::string uuid = get_uuid(10);

        logger.log("New session <" + uuid + ">", Level::INFO);

        if(service == nullptr) { // если мод не был выбран
            service = raplMap[RaplMode::PKG];
            current_mode = RaplMode::PKG;
            logger.log("<" + uuid + "> Set default mode: PKG", Level::WARN);
        }

        // Обработка запросов
        while (true) {
            // Получение запроса от клиента
            char buffer[1024];
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) {
                break;
            }
            request = std::string(buffer, bytesReceived);
            logger.log("<" + uuid + "> Request: " + request , Level::INFO);

            // Обработка запроса
            if (request == "start") {
                service->start_measure();
                // Отправка ответа клиенту
                std::string response = "OK";
                logger.log("<" + uuid + "> Starting measure", Level::INFO);

                send(clientSocket, response.c_str(), response.length(), 0);
            } else if (request == "stop") {
                service->complete_measure();

                logger.log("<" + uuid + "> Measure completed", Level::INFO);

                auto result = service->total_energy();
                // Отправка ответа клиенту
                std::string response = vectorToString(result);
                send(clientSocket, response.c_str(), response.length(), 0);

                if(current_mode == RaplMode::ALL_CORES) {
                    logger.log("<" + uuid + "> All cores: ", Level::INFO);
                    logger.log("<" + uuid + "> ", result, Level::INFO);
                } else {
                    logger.log("<" + uuid + "> Measure result: " + response, Level::INFO);
                }
            } else if (request == "PKG") {
                changeMode(RaplMode::PKG);
                current_mode = RaplMode::PKG;

                std::string response = "Mode: PKG";
                send(clientSocket, response.c_str(), response.length(), 0);

                logger.log("<" + uuid + "> Change Rapl mode: PKG", Level::WARN);
            } else if (request == "SINGLE_CORE") {
                changeMode(RaplMode::SINGLE_CORE);
                current_mode = RaplMode::SINGLE_CORE;

                std::string response = "Mode: SINGLE_CORE";
                send(clientSocket, response.c_str(), response.length(), 0);

                logger.log("<" + uuid + "> Change Rapl mode: SINGLE_CORE", Level::WARN);
            } else if (request == "ALL_CORES") {
                changeMode(RaplMode::ALL_CORES);
                current_mode = RaplMode::ALL_CORES;

                std::string response = "Mode: ALL_CORES";
                send(clientSocket, response.c_str(), response.length(), 0);

                logger.log("<" + uuid + "> Change Rapl mode: ALL_CORES", Level::WARN);
            } else {
                // Неизвестный запрос
                std::string response = "Invalid request";
                send(clientSocket, response.c_str(), response.length(), 0);

                logger.log("<" + uuid + "> Invalid request type", Level::WARN);
            }
        }

        // Закрытие соединения с клиентом
        close(clientSocket);
        logger.log("<" + uuid + "> Connection closed", Level::INFO);
    }

    // Закрытие сокета сервера
    close(serverSocket);
    logger.log("Socket closed", Level::INFO);
}

std::string RaplDeamon::get_uuid(int length = 10) {
    std::stringstream ss;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 15);

    for (int i = 0; i < length; ++i) {
        int randomHexDigit = distribution(generator);
        ss << std::hex << std::setfill('0') << std::setw(1) << randomHexDigit;
    }

    return ss.str();
}

void RaplDeamon::changeMode(RaplMode mode) {
    service = raplMap[mode];
}

std::unordered_map<std::string, std::string> RaplDeamon::read_config(const std::string& filename) {
    std::unordered_map<std::string, std::string> config;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return config;
    }

    std::string line;
    while (getline(file, line)) {
        // Пропускаем комментарии
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Разделяем строку на ключ и значение
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "Неверный формат строки: " << line << std::endl;
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        key.erase(remove(key.begin(), key.end(), ' '), key.end());
        value.erase(remove(value.begin(), value.end(), ' '), value.end());

        config[key] = value;
    }

    file.close();
    return config;
}


/////////////////
/////////////////

void signal_handler(int sig) {
  Logger& logger = Logger::getInstance();
  // Обработка сигнала
  switch (sig) {
    case SIGINT:
    case SIGTERM:
    case SIGQUIT:
        exit(0);
        break;
    default:
        break;
  }
}


int becomeDeamon() {
    // Создание дочернего процесса
    switch (fork()) {    /* Превращение в фоновый процесс */
    case -1: return -1;
    case 0:
    break;
    /* Потомок проходит этот этап... */
    default: _exit(EXIT_SUCCESS); /* ...а родитель завершается */
    }

    // Отделение от терминала
    setsid();
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Запись PID в файл
    std::string pid_file = "/var/run/unirapl.pid";
    FILE* fp = fopen(pid_file.c_str(), "w");
    if (fp == NULL) {
        return -1;
     }
    fprintf(fp, "%d\n", getpid());
    fclose(fp);

    // //Блокировка файла PID
    // flock(fileno(fp), LOCK_EX | LOCK_NB);

    //Обработка сигналов
    signal(SIGCHLD, SIG_IGN); // Игнорируем сигнал о завершении дочернего процесса
    signal(SIGINT, signal_handler);  // Завершение по умолчанию при Ctrl+C
    signal(SIGTERM, signal_handler); // Завершение по умолчанию при kill
    signal(SIGQUIT, signal_handler);

    return 0;
}
