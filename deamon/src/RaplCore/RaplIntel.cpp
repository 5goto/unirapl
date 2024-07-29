#include "Rapl.h"
#include <cmath>


void RaplIntel::architecture_init() {
    config_.POWER_UNIT_MASK = 0xf;
    config_.TIME_UNIT_MASK = 0xf;
    config_.ENERGY_UNIT_MASK = 0x1f;
    config_.MSR_PWR_UNIT = 0x606;
    config_.MSR_PACKAGE_ENERGY = 0x611;
    config_.MSR_CORE_ENERGY = 0x639;

    uint64_t core_energy_units = read_msr(fd, config_.MSR_PWR_UNIT);

   	power_unit = (double) (core_energy_units & config_.POWER_UNIT_MASK);
    energy_unit = (double) ((core_energy_units >> 8) & config_.ENERGY_UNIT_MASK);
    time_unit = (double) ((core_energy_units >> 16) & config_.TIME_UNIT_MASK);

    time_unit_d = std::scalbn(0.5, -time_unit + 1);
  	energy_unit_d = std::scalbn(0.5, -energy_unit + 1);
  	power_unit_d = std::scalbn(0.5, -power_unit + 1);
}

RaplIntel_PKG::RaplIntel_PKG() {
    fd = open_msr(current_core);
    architecture_init();
}

RaplIntel_SINGLE_CORE::RaplIntel_SINGLE_CORE(int core_num) {
    fd = open_msr(current_core);

    architecture_init();
    detect_cpu_topology();

    if (core_num < 0 || core_num > topology.num_cores - 1) {
        current_core = 0;
    } else {
        current_core = core_num;
    }
}

void RaplIntel_SINGLE_CORE::start_measure() {
   	energy_acc_first = read_msr(fd, config_.MSR_CORE_ENERGY);
	energy_acc_second = 0;
}

void RaplIntel_SINGLE_CORE::complete_measure() {
    energy_acc_second = read_msr(fd, config_.MSR_CORE_ENERGY);
	measure_result = energy_delta(energy_acc_first, energy_acc_second);
}

RaplIntel_ALL_CORES::RaplIntel_ALL_CORES() {
    detect_cpu_topology();

    fd_cores = new int[topology.num_cores / 2];
    for (int core = 0; core < topology.num_cores / 2; core++) {
        fd_cores[core] = open_msr(core);
    }

    fd = fd_cores[0];
    architecture_init();

    energy_acc_first_arr = new uint64_t[topology.num_cores / 2];
    energy_acc_second_arr = new uint64_t[topology.num_cores / 2];
}

RaplIntel_ALL_CORES::~RaplIntel_ALL_CORES() {
    delete [] fd_cores;
    delete [] energy_acc_first_arr;
    delete [] energy_acc_second_arr;
}

void RaplIntel_ALL_CORES::start_measure() {
    for (int core = 0; core < topology.num_cores / 2; core++) {
        energy_acc_first_arr[core] = read_msr(fd_cores[core], config_.MSR_CORE_ENERGY);
    }
}

void RaplIntel_ALL_CORES::complete_measure() {
    for (int core = 0; core < topology.num_cores / 2; core++) {
        energy_acc_second_arr[core] = read_msr(fd_cores[core], config_.MSR_CORE_ENERGY);
    }
}

std::vector<double> RaplIntel_ALL_CORES::total_energy() {
    std::vector<double> result;

    for (int core = 0; core < topology.num_cores / 2; core++) {
        result.push_back(energy_unit_d * (double)energy_delta(energy_acc_first_arr[core], energy_acc_second_arr[core]));
    }
    return result;
}
