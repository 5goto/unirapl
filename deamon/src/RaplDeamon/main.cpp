#include "RaplDeamon.h"
#include "../RaplCore/Rapl.h"
#include <cstdlib>

int main() {
    std::unordered_map<std::string, std::string> config = RaplDeamon::read_config("conf/config.txt");



    if (becomeDeamon() < 0) {
        exit(1);
    }

    std::string arch = Rapl::get_arch_by_cpuinfo();
    std::unordered_map<RaplMode, Rapl*> raplMap;


    try {
        if (arch == "AMD") {
            raplMap[RaplMode::PKG] = new RaplAMD_PKG();
            raplMap[RaplMode::SINGLE_CORE] = new RaplAMD_SINGLE_CORE(std::stoi(config["core"]));
            raplMap[RaplMode::ALL_CORES] = new RaplAMD_ALL_CORES();
        } else if (arch == "INTEL") {
            raplMap[RaplMode::PKG] = new RaplIntel_PKG();
            raplMap[RaplMode::SINGLE_CORE] = new RaplIntel_SINGLE_CORE(std::stoi(config["core"]));
            raplMap[RaplMode::ALL_CORES] = new RaplIntel_ALL_CORES();
        } else {
            exit(1);
        }

        RaplDeamon deamon(raplMap);
        deamon.run(std::stoi(config["port"]));
    }
    catch(const std::exception& e){   
        exit(1);
    }

    return 0;
}
