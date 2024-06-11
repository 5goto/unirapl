#include <unistd.h>
#include <cstdint>
#include <sys/time.h>

constexpr int MAX_PACKAGES = 16;
constexpr int MAX_CPUS = 1024;

struct rapl_state_t {
	uint64_t* pkg;
	uint64_t* pp0;
};

struct msr_config {
    uint64_t MSR_PWR_UNIT;
    uint64_t TIME_UNIT_MASK;
    uint64_t ENERGY_UNIT_MASK;
    uint64_t POWER_UNIT_MASK;
    uint64_t MSR_PACKAGE_ENERGY;
    uint64_t MSR_CORE_ENERGY;
};

class Rapl {
private:

    Rapl();
	~Rapl();

protected:
    msr_config config_;

    static Rapl* instance; // singleton ex


    int fd;
    std::string architecture;
    std::string target;
    unsigned int time_unit, energy_unit, power_unit;
	double time_unit_d, energy_unit_d, power_unit_d;

    // Rapl state
	rapl_state_t *current_state;
	rapl_state_t *prev_state;
	rapl_state_t *next_state;
	rapl_state_t state1, state2, state3, running_total;

    unsigned current_core = -1;

    int total_cores=0;
	int total_packages=0;
    int package_map[MAX_PACKAGES];

    // void init_amd();
    // void init_intel();
    void get_architecture(RaplConfig*);
    void get_target(RaplConfig*);
    void get_core(RaplConfig*);
    void get_msr(RaplConfig*);

    uint64_t energy_delta(uint64_t* before, uint64_t* after);
    uint64_t read_msr(int fd, unsigned int msr_offset);

public:

    static Rapl* get_instance() {
			if (instance == nullptr) {
				instance = new Rapl();
			}
        return instance;
    }

    void reset();
	void sample();
	int detect_packages();

	int open_msr(int core);

	void free_state();
	double total_energy();
};