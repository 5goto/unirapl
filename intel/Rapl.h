#include <unistd.h>
#include <cstdint>
#include <sys/time.h>

#ifndef RAPL_H_
#define RAPL_H_

#define MAX_PACKAGES	16

struct rapl_state_t {
	uint64_t pkg;
	uint64_t pp0;
	uint64_t pp1;
	uint64_t dram;
	struct timeval tsc;
};

class Rapl {

private:
	static Rapl* instance; // singlton instance
	// Rapl configuration
	Rapl();
	Rapl(unsigned core_index);

	int fd;
	int* fd_cores;
	int core = 0;
	bool pp1_supported = true;
	double power_units, energy_units, time_units;
	double thermal_spec_power, minimum_power, maximum_power, time_window;

	int total_cores=0;
	int total_packages=0;
	int package_map[MAX_PACKAGES];
	unsigned current_core = -1;

	// Rapl state
	rapl_state_t *current_state;
	rapl_state_t *prev_state;
	rapl_state_t *next_state;
	rapl_state_t state1, state2, state3, running_total;

	void open_msr();
	int open_msr(int core);
	uint64_t read_msr(int msr_offset);
	uint64_t read_msr(int fd, unsigned int msr_offset);
	double time_delta(struct timeval *begin, struct timeval *after);
	uint64_t energy_delta(uint64_t before, uint64_t after);
	double power(uint64_t before, uint64_t after, double time_delta);

public:
	static Rapl* get_instance() {
			if (instance == nullptr) {
				instance = new Rapl();
			}
        return instance;
    }

		static Rapl* get_instance_core(unsigned core_index) {
			if (instance == nullptr) {
				instance = new Rapl(core_index);
			}
        return instance;
    }

	void reset();
	void reset_core();
	void sample();
	void sample_core();

	double pkg_total_energy();
	double pp0_total_energy();

	void free_state();
	void sample_core();

	int detect_packages();
};

#endif /* RAPL_H_ */
