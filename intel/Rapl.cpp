#include <cstdio>
#include <string>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <cmath>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "Rapl.h"

#define MSR_RAPL_POWER_UNIT            0x606

/*
 * Platform specific RAPL Domains.
 * Note that PP1 RAPL Domain is supported on 062A only
 * And DRAM RAPL Domain is supported on 062D only
 */
/* Package RAPL Domain */
#define MSR_PKG_RAPL_POWER_LIMIT       0x610
#define MSR_PKG_ENERGY_STATUS          0x611
#define MSR_PKG_PERF_STATUS            0x13
#define MSR_PKG_POWER_INFO             0x614

/* PP0 RAPL Domain */
#define MSR_PP0_POWER_LIMIT            0x638
#define MSR_PP0_ENERGY_STATUS          0x639
#define MSR_PP0_POLICY                 0x63A
#define MSR_PP0_PERF_STATUS            0x63B

/* PP1 RAPL Domain, may reflect to uncore devices */
#define MSR_PP1_POWER_LIMIT            0x640
#define MSR_PP1_ENERGY_STATUS          0x641
#define MSR_PP1_POLICY                 0x642

/* DRAM RAPL Domain */
#define MSR_DRAM_POWER_LIMIT           0x618
#define MSR_DRAM_ENERGY_STATUS         0x619
#define MSR_DRAM_PERF_STATUS           0x61B
#define MSR_DRAM_POWER_INFO            0x61C

/* RAPL UNIT BITMASK */
#define POWER_UNIT_OFFSET              0
#define POWER_UNIT_MASK                0x0F

#define ENERGY_UNIT_OFFSET             0x08
#define ENERGY_UNIT_MASK               0x1F00

#define TIME_UNIT_OFFSET               0x10
#define TIME_UNIT_MASK                 0xF000

#define SIGNATURE_MASK                 0xFFFF0
#define IVYBRIDGE_E                    0x306F0
#define SANDYBRIDGE_E                  0x206D0

#define MAX_CPUS	1024
#define MAX_PACKAGES	16


Rapl* Rapl::instance = nullptr;

Rapl::Rapl() {

	open_msr();

	/* Read MSR_RAPL_POWER_UNIT Register */
	uint64_t raw_value = read_msr(MSR_RAPL_POWER_UNIT);
	power_units = pow(0.5,	(double) (raw_value & 0xf));
	energy_units = pow(0.5,	(double) ((raw_value >> 8) & 0x1f));
	time_units = pow(0.5,	(double) ((raw_value >> 16) & 0xf));

	/* Read MSR_PKG_POWER_INFO Register */
	raw_value = read_msr(MSR_PKG_POWER_INFO);
	thermal_spec_power = power_units * ((double)(raw_value & 0x7fff));
	minimum_power = power_units * ((double)((raw_value >> 16) & 0x7fff));
	maximum_power = power_units * ((double)((raw_value >> 32) & 0x7fff));
	time_window = time_units * ((double)((raw_value >> 48) & 0x7fff));

	reset();
}

Rapl::Rapl(unsigned core_index) {
	detect_packages();

	fd_cores = (int*)malloc(sizeof(int)*total_cores/2);

	for (int i = 0; i < total_cores/2; i++) {
		fd_cores[i] = open_msr(i);
	}

	/* Read MSR_RAPL_POWER_UNIT Register */
	uint64_t raw_value = read_msr(MSR_RAPL_POWER_UNIT);
	power_units = pow(0.5,	(double) (raw_value & 0xf));
	energy_units = pow(0.5,	(double) ((raw_value >> 8) & 0x1f));
	time_units = pow(0.5,	(double) ((raw_value >> 16) & 0xf));

	/* Read MSR_PKG_POWER_INFO Register */
	raw_value = read_msr(MSR_PKG_POWER_INFO);
	thermal_spec_power = power_units * ((double)(raw_value & 0x7fff));
	minimum_power = power_units * ((double)((raw_value >> 16) & 0x7fff));
	maximum_power = power_units * ((double)((raw_value >> 32) & 0x7fff));
	time_window = time_units * ((double)((raw_value >> 48) & 0x7fff));

	reset_core();
}

void Rapl::reset() {

	prev_state = &state1;
	current_state = &state2;
	next_state = &state3;

	// sample twice to fill current and previous
	sample();
	sample();

	// Initialize running_total
	running_total.pkg = 0;
}


void Rapl::reset_core() {

	prev_state = &state1;
	current_state = &state2;
	next_state = &state3;

	// sample twice to fill current and previous
	sample_core();
	sample_core();

	// Initialize running_total
	running_total.pp0 = 0;
}

void Rapl::free_state(){
	// Initialize running_total
	if (current_core == -1) {
		sample();
		running_total.pkg = 0;
	} else {
		sample_core();
		running_total.pp0 = 0;
	}
}


void Rapl::open_msr() {
	std::stringstream filename_stream;
	filename_stream << "/dev/cpu/" << core << "/msr";
	fd = open(filename_stream.str().c_str(), O_RDONLY);
	if (fd < 0) {
		if ( errno == ENXIO) {
			fprintf(stderr, "rdmsr: No CPU %d\n", core);
			exit(2);
		} else if ( errno == EIO) {
			fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n", core);
			exit(3);
		} else {
			perror("rdmsr:open");
			fprintf(stderr, "Trying to open %s\n",
					filename_stream.str().c_str());
			exit(127);
		}
	}
}


int Rapl::open_msr(int core) {

	char msr_filename[BUFSIZ];
	int fd;

	sprintf(msr_filename, "/dev/cpu/%d/msr", core);
	fd = open(msr_filename, O_RDONLY);
	if ( fd < 0 ) {
		if ( errno == ENXIO ) {
			fprintf(stderr, "rdmsr: No CPU %d\n", core);
			exit(2);
		} else if ( errno == EIO ) {
			fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n", core);
			exit(3);
		} else {
			perror("rdmsr:open");
			fprintf(stderr,"Trying to open %s\n",msr_filename);
			exit(127);
		}
	}

	return fd;
}

uint64_t Rapl::read_msr(int msr_offset) {
	uint64_t data;
	if (pread(fd, &data, sizeof(data), msr_offset) != sizeof(data)) {
		perror("read_msr():pread");
		exit(127);
	}
	return data;
}

uint64_t Rapl::read_msr(int fd, unsigned int msr_offset) {

	uint64_t data;

	if ( pread(fd, &data, sizeof data, msr_offset) != sizeof data ) {
		perror("rdmsr:pread");
		exit(127);
	}

	return data;
}

void Rapl::sample() {
	uint32_t max_int = ~((uint32_t) 0);

	next_state->pkg = read_msr(MSR_PKG_ENERGY_STATUS) & max_int;

	// Update running total
	running_total.pkg += energy_delta(current_state->pkg, next_state->pkg);

	// Rotate states
	rapl_state_t *pprev_state = prev_state;
	prev_state = current_state;
	current_state = next_state;
	next_state = pprev_state;
}


void Rapl::sample_core() {
	uint32_t max_int = ~((uint32_t) 0);


	next_state->pp0 = read_msr(fd_cores[current_core], MSR_PP0_ENERGY_STATUS) & max_int;
	running_total.pp0 += energy_delta(current_state->pp0, next_state->pp0);

	// Rotate states
	rapl_state_t *pprev_state = prev_state;
	prev_state = current_state;
	current_state = next_state;
	next_state = pprev_state;
}



double Rapl::time_delta(struct timeval *begin, struct timeval *end) {
        return (end->tv_sec - begin->tv_sec)
                + ((end->tv_usec - begin->tv_usec)/1000000.0);
}

double Rapl::power(uint64_t before, uint64_t after, double time_delta) {
	if (time_delta == 0.0f || time_delta == -0.0f) { return 0.0; }
	double energy = energy_units * ((double) energy_delta(before,after));
	return energy / time_delta;
}

uint64_t Rapl::energy_delta(uint64_t before, uint64_t after) {
	uint64_t max_int = ~((uint32_t) 0);
	uint64_t eng_delta = after - before;

	// Check for rollovers
	if (before > after) {
		eng_delta = after + (max_int - before);
	}

	return eng_delta;
}


double Rapl::pkg_total_energy() {
	return energy_units * ((double) running_total.pkg);
}

double Rapl::pp0_total_energy() {
	return energy_units * ((double) running_total.pp0);
}


int Rapl::detect_packages() {

	char filename[BUFSIZ];
	FILE *fff;
	int package;
	int i;

	for(i=0;i<MAX_PACKAGES;i++) package_map[i]=-1;

	//printf("\t");
	for(i=0;i<MAX_CPUS;i++) {
		sprintf(filename,"/sys/devices/system/cpu/cpu%d/topology/physical_package_id",i);
		fff=fopen(filename,"r");
		if (fff==NULL) break;
		fscanf(fff,"%d",&package);
		//printf("%d (%d)",i,package);
		//if (i%8==7) printf("\n\t"); else printf(", ");
		fclose(fff);

		if (package_map[package]==-1) {
			total_packages++;
			package_map[package]=i;
		}
	}

	//printf("\n");

	total_cores=i;

	//printf("\tDetected %d cores in %d packages\n\n", total_cores,total_packages);
	return 0;
}