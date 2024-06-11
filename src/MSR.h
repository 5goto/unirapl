#include <cstdint>

struct config {
    uint64_t MSR_PWR_UNIT;
    uint64_t TIME_UNIT_MASK;
    uint64_t ENERGY_UNIT_MASK;
    uint64_t POWER_UNIT_MASK;
    uint64_t MSR_PACKAGE_ENERGY;
    uint64_t MSR_CORE_ENERGY;
};

struct config_amd : config {
    uint64_t MSR_PWR_UNIT = 0xC0010299;
    uint64_t TIME_UNIT_MASK = 0xF0000;
    uint64_t ENERGY_UNIT_MASK = 0x1F00;
    uint64_t POWER_UNIT_MASK = 0xF;
    uint64_t MSR_PACKAGE_ENERGY = 0xC001029B;
    uint64_t MSR_CORE_ENERGY = 0xC001029A;
};

struct config_intel : config {
    uint64_t MSR_PWR_UNIT = 0x606;
    uint64_t TIME_UNIT_MASK = 0xf;
    uint64_t ENERGY_UNIT_MASK = 0x1f;
    uint64_t POWER_UNIT_MASK = 0xf;
    uint64_t MSR_PACKAGE_ENERGY = 0x611;
    uint64_t MSR_CORE_ENERGY = 0x639;
};