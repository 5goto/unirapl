#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <random>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <algorithm>
#include "../RaplCore/Rapl.h"
#include "RaplLogger.h"


class RaplDeamon {
public:
    RaplDeamon(std::unordered_map<RaplMode, Rapl*> raplMap) : raplMap(raplMap) {}
    ~RaplDeamon();

    void run(int port);
    static std::unordered_map<std::string, std::string> read_config(const std::string& filename);


private:
    int serverSocket, clientSocket;

    Rapl* service = nullptr;
    std::unordered_map<RaplMode, Rapl*> raplMap;

    Logger& logger = Logger::getInstance();

    std::string get_uuid(int);

    RaplMode current_mode;
    void changeMode(RaplMode mode);
};

int becomeDeamon();
void signal_handler(int);
