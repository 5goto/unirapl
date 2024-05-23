#include "proper_interface.h"
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MSR_RAPL_POWER_UNIT            0x606
#define MSR_PKG_RAPL_POWER_LIMIT       0x610
#define MSR_PKG_ENERGY_STATUS 0x611
#define MSR_PP0_ENERGY_STATUS          0x639

#define TOSTR_(x) #x
#define TOSTR(x) TOSTR_(x)
#define fail(where) do {fprintf(stderr, "Failure at %s (%u): %s\n", __FILE__, __LINE__, TOSTR(where)); exit(__LINE__);} while (0)
#define check(f) if (!(f)) fail(TOSTR(f))

static unsigned long long read_msr(unsigned core, int msr) {
	char msr_path[PATH_MAX];
	unsigned long long val;
	int f;
	sprintf(msr_path, "/dev/cpu/%u/msr", core);
	f = open(msr_path, O_RDONLY);
	if (f < 0)
		return ULLONG_MAX;
	if (pread(f, &val, sizeof val, msr) < 0) {
		close(f);
		return ULLONG_MAX;
	}
	close(f);
	return val;
}

typedef struct invariant_data_ {
	double cpu_units;
	unsigned long long max_energy;
} invariant_data_t;

static invariant_data_t init() {
	unsigned long long unit = read_msr(0, MSR_RAPL_POWER_UNIT);
	invariant_data_t result;
	check(unit != ULLONG_MAX);
	result.cpu_units = scalbn(1., 1 - (int) ((unit >> 8) & 0x1Full));
	result.max_energy = 1ll << 32; //плохо поддерживается read_msr(0, MSR_PKG_RAPL_POWER_LIMIT)
	return result;
}

static invariant_data_t inv;

energy_handle begin_energy_measurement() {
	if (!inv.max_energy)
		inv = init();
	return read_msr(0, MSR_PKG_ENERGY_STATUS) & UINT_MAX;
}

#define energy_diff(e1, e2, emax, units) (((double) ((e2) >= (e1)?(e2) - (e1):(e2) - (e1) + (emax))) * units)

double complete_energy_measurement(energy_handle h) {
	unsigned long long r = read_msr(0, MSR_PKG_ENERGY_STATUS) & UINT_MAX;
	return energy_diff(h, r, inv.max_energy, inv.cpu_units);
}

