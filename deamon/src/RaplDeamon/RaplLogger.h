#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <fstream>
#include <unistd.h>
#include <vector>

enum class Level { DEBUG, INFO, WARN, ERROR };

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        instance.init();
        return instance;
    }

    void log(const std::string message, Level level);
    void log(const std::string message, std::vector<double> vec, Level level);
    void setLevel(Level level);
    bool isEnabled(Level level);
    void init();

private:
    Level _level = Level::INFO;
    std::string log_file_path = "/var/log/unirapl.log";
    std::ofstream logFile;

    Logger() {}
    ~Logger() {
           if (logFile.is_open()) {
               logFile.close();
           }
       }
    std::string getCurrentTimestamp();
    std::string levelToString(Level);
};
