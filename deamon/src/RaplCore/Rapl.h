#pragma once

#include <unistd.h>
#include <cstdint>
#include <sys/time.h>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

constexpr int MAX_PACKAGES = 16;

struct msr_config {
    uint64_t MSR_PWR_UNIT;
    uint64_t TIME_UNIT_MASK;
    uint64_t ENERGY_UNIT_MASK;
    uint64_t POWER_UNIT_MASK;
    uint64_t MSR_PACKAGE_ENERGY;
    uint64_t MSR_CORE_ENERGY;
};

struct CpuTopology {
  int num_packages;
  int num_cores;
};

class RaplMainInterface {
    protected:
        virtual void start_measure() = 0;
        virtual void complete_measure() = 0;
        virtual std::vector<double> total_energy() = 0;
};

enum class RaplMode {PKG, SINGLE_CORE, ALL_CORES};

class Rapl : public RaplMainInterface {
protected:
    msr_config config_;
    CpuTopology topology;

    int fd;
    double time_unit, energy_unit, power_unit;
	double time_unit_d, energy_unit_d, power_unit_d;

	uint64_t energy_acc_first, energy_acc_second;
    uint64_t measure_result;

    unsigned current_core = 0;

    uint64_t energy_delta(uint64_t before, uint64_t after);
    uint64_t read_msr(int fd, unsigned int msr_offset);

	int open_msr(int core);
	void detect_cpu_topology();

	virtual void architecture_init() = 0;
public:
	virtual void start_measure() override;
	virtual void complete_measure() override;

	virtual std::vector<double> total_energy() override;

	static std::string get_arch_by_cpuinfo();
};


class RaplAMD : public Rapl {
    protected:
        virtual void architecture_init() override;
};

class RaplAMD_PKG : public RaplAMD {
    public:
        RaplAMD_PKG();
};

class RaplAMD_SINGLE_CORE : public RaplAMD {
    public:
        RaplAMD_SINGLE_CORE(int);
        void start_measure() override;
        void complete_measure() override;
};

class RaplAMD_ALL_CORES : public RaplAMD {
    private:
        uint64_t* energy_acc_first_arr;
        uint64_t* energy_acc_second_arr;
        int* fd_cores;
    public:
        RaplAMD_ALL_CORES();
        ~RaplAMD_ALL_CORES();

        void start_measure() override;
        void complete_measure() override;
        std::vector<double> total_energy() override;
};

class RaplIntel : public Rapl {
    protected:
        virtual void architecture_init() override;
};

class RaplIntel_PKG : public RaplIntel {
    public:
        RaplIntel_PKG();
};

class RaplIntel_SINGLE_CORE : public RaplIntel {
    public:
        RaplIntel_SINGLE_CORE(int);
        void start_measure() override;
        void complete_measure() override;
};

class RaplIntel_ALL_CORES : public RaplIntel {
    private:
        uint64_t* energy_acc_first_arr;
        uint64_t* energy_acc_second_arr;
        int* fd_cores;
    public:
        RaplIntel_ALL_CORES();
        ~RaplIntel_ALL_CORES();

        void start_measure() override;
        void complete_measure() override;
        std::vector<double> total_energy() override;
};
