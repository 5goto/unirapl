#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <exception>
#include "RaplConfig.h"



// Функция для парсинга текстового файла конфигурации
void RaplConfig::parseConfig() {
    std::ifstream file(config_name);
    if (!file.is_open()) {
        throw std::runtime_error("Невозможно открыть файл конфигурации");
    }
    std::string line;
    while (getline(file, line)) {
        // Разделить строку по двоеточию
        size_t pos = line.find(':');
        if (pos == std::string::npos) {
            continue;
        }
        // Получить ключ и значение
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Добавить ключ и значение в map
        config[key] = value;
    }
}

uint64_t RaplConfig::get_msr(std::string msr) {
    return std::stoul(config[msr], nullptr, 16);
}

bool RaplConfig::get_bool(std::string val) {
    std::string str = config[val];

    if (str == "true") {
        return true;
    } else if (str == "false") {
        return false;
    } else {
        throw std::runtime_error("Некорректый ключ в конфиге");
    }
}

int RaplConfig::get_number(std::string num) {
    return stoi(config[num]);
}

std::string RaplConfig::get_string(std::string str) {
    return config[str];
}
