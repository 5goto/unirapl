#include "RaplLogger.h"


void Logger::log(const std::string message, Level level) {
    if (isEnabled(level)) {
        if (logFile.is_open()) {
            logFile << getCurrentTimestamp() << " [" << levelToString(level) << "] " << message << std::endl;
        } else {
            std::cerr << "Error writing to log file: " << log_file_path << std::endl;
        }
    }
}

void Logger::log(const std::string message, std::vector<double> vec, Level level) {
    if (isEnabled(level)) {
        if (logFile.is_open()) {
            int num_count = 0;
            for (double val: vec) {
                logFile << getCurrentTimestamp() << " [" << levelToString(level) << "] " << message <<
                "[" << num_count << "] " << val << std::endl;
                num_count++;
            }
        } else {
            std::cerr << "Error writing to log file: " << log_file_path << std::endl;
        }
    }
}

void Logger::setLevel(Level level) {
    _level = level;
}

bool Logger::isEnabled(Level level) {
    return level >= _level;
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm* ptm = std::localtime(&tt);

    std::stringstream ss;
    ss << std::put_time(ptm, "%Y-%m-%d_%H:%M:%S") << ":" << (millis % 1000);

    return ss.str();
}

void Logger::init() {
    struct stat sb;
    // Проверка существования файла
    if (stat(log_file_path.c_str(), &sb) == -1) {
    // Если файл не существует, создаем его
        logFile.open(log_file_path);
        if (!logFile.is_open()) {
            std::cerr << "Error creating log file: " << log_file_path << std::endl;
        }
    } else {
              // Если файл существует, открываем на дозапись
        logFile.open(log_file_path, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Error opening log file: " << log_file_path << std::endl;
            }
        }

          // Устанавливаем права доступа для остальных пользователей
        if (chmod(log_file_path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == -1) {
            std::cerr << "Error setting permissions for log file: " << log_file_path << std::endl;
        }
}

std::string Logger::levelToString(Level level) {
        switch (level) {
            case Level::DEBUG: return "DEBUG";
            case Level::INFO: return "INFO";
            case Level::WARN: return "WARN";
            case Level::ERROR: return "ERROR";
        }
        return "UNKNOWN";
    }
