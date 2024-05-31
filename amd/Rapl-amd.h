#include <sys/time.h>
#include <cstdint>


#define MAX_PACKAGES	16

struct rapl_state_t {
	uint64_t* pkg;
	uint64_t* pp0;
	struct timeval tsc;
};

struct total_state_t {
	double pkg;
	double pp0;
	struct timeval tsc;
};

class Rapl {
	private:
		static Rapl* instance;

		Rapl();

		int* fd;

		unsigned int time_unit, energy_unit, power_unit;
		double time_unit_d, energy_unit_d, power_unit_d;


		int total_cores=0;
		int total_packages=0;
		int package_map[MAX_PACKAGES];

		unsigned current_core = -1;

			// Rapl state
		rapl_state_t *current_state;
		rapl_state_t *prev_state;
		rapl_state_t *next_state;
		rapl_state_t state1, state2, state3;

		total_state_t running_total;

		uint64_t read_msr(int fd, unsigned int msr_offset);
		double time_delta(struct timeval *begin, struct timeval *after);
		uint64_t energy_delta(uint64_t* before, uint64_t* after);
        uint64_t energy_delta_pkg(uint64_t* before, uint64_t* after);
		double power(uint64_t* before, uint64_t* after, double time_delta);
        double power_pkg(uint64_t* before, uint64_t* after, double time_delta);

	public:
		static Rapl* get_instance() {
        if (instance == nullptr) {
            instance = new Rapl();
        }
        return instance;
    }


		//Rapl();
		//Rapl(unsigned core_index);
		void reset();
		void sample();
		void sample_core();
		int detect_packages(void);
		int open_msr(int core);

		void free_state();

		double total_time();
		double current_time();

		double pkg_current_power();
		double pp0_current_power();

		double pkg_total_energy();
		double pp0_total_energy();
};