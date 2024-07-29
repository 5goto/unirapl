#include "RaplDeamon.h"
#include <cstdlib>

int main() {
    Logger& logger = Logger::getInstance();
    logger.log("The process is running", Level::INFO);

    logger.log("Trying to read config.txt", Level::INFO);
    std::unordered_map<std::string, std::string> config = RaplDeamon::read_config("conf/config.txt");

    for (const auto& pair : config) {
        logger.log("KEY: " + pair.first + " VALUE: " + pair.second, Level::INFO);
    }

    if (becomeDeamon() < 0) {
        logger.log("Error in the process of becoming a demon", Level::ERROR);
        exit(1);
    }

    std::string arch = Rapl::get_arch_by_cpuinfo();
    logger.log("Architecture identification: " + arch, Level::INFO);

    std::unordered_map<RaplMode, Rapl*> raplMap;

    if (arch == "AMD") {
        raplMap[RaplMode::PKG] = new RaplAMD_PKG();
        raplMap[RaplMode::SINGLE_CORE] = new RaplAMD_SINGLE_CORE(std::stoi(config["core"]));
        raplMap[RaplMode::ALL_CORES] = new RaplAMD_ALL_CORES();
    } else if (arch == "INTEL") {
        raplMap[RaplMode::PKG] = new RaplIntel_PKG();
        raplMap[RaplMode::SINGLE_CORE] = new RaplIntel_SINGLE_CORE(std::stoi(config["core"]));
        raplMap[RaplMode::ALL_CORES] = new RaplIntel_ALL_CORES();
    } else {
        logger.log("Architecture identification FAILED. Exiting..." + arch, Level::ERROR);
        exit(1);
    }

    RaplDeamon deamon(raplMap);
    deamon.run(std::stoi(config["port"]));

    return 0;
}
