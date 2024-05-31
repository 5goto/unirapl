
#include <sys/time.h>


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <math.h>
#include <cmath>
#include <string.h>

#include <sys/syscall.h>
#include <linux/perf_event.h>

#include "Rapl-amd.h"

//#include "Rapl.h"

// ENERGY PWR UNIT MSR
#define AMD_MSR_PWR_UNIT 0xC0010299
// ENERGY CORE MSR	
#define AMD_MSR_CORE_ENERGY 0xC001029A
// ENERGY_PKG_MSR
#define AMD_MSR_PACKAGE_ENERGY 0xC001029B

#define AMD_TIME_UNIT_MASK 0xF0000
#define AMD_ENERGY_UNIT_MASK 0x1F00
#define AMD_POWER_UNIT_MASK 0xF
#define STRING_BUFFER 1024

#define MAX_CPUS	1024
#define MAX_PACKAGES	16

Rapl* Rapl::instance = nullptr;

Rapl::Rapl() {
	detect_packages();


	fd = (int*)malloc(sizeof(int)*total_cores/2);

	for (int i = 0; i < total_cores/2; i++) {
		fd[i] = open_msr(i);
	}

    int core_energy_units = read_msr(fd[0], AMD_MSR_PWR_UNIT);
	//printf("Core energy units: %x\n",core_energy_units);
	
	time_unit = (core_energy_units & AMD_TIME_UNIT_MASK) >> 16;
	energy_unit = (core_energy_units & AMD_ENERGY_UNIT_MASK) >> 8;
	power_unit = (core_energy_units & AMD_POWER_UNIT_MASK);
	//printf("Time_unit:%d, Energy_unit: %d, Power_unit: %d\n", time_unit, energy_unit, power_unit);
	
	time_unit_d = pow(0.5,(double)(time_unit));
	energy_unit_d = pow(0.5,(double)(energy_unit));
	power_unit_d = pow(0.5,(double)(power_unit));

	// time_unit_d = std::scalbn(0.5, time_unit);
	// energy_unit_d = std::scalbn(0.5, energy_unit);
	// power_unit_d = std::scalbn(0.5, power_unit);

	// printf("Time_unit:%g, Energy_unit: %g, Power_unit: %g\n", time_unit_d, energy_unit_d, power_unit_d);
	reset();
}

// Rapl::Rapl(unsigned core_index) {
// 	detect_packages();

// 	if(core_index < 0 || core_index > total_cores/2) {
// 		printf("Avialible cors: [%d:%d]", 0, total_cores/2 - 1);
// 		exit(1);
// 	}

// 	current_core = core_index;

// 	fd = (int*)malloc(sizeof(int)*total_cores/2);

// 	for (int i = 0; i < total_cores/2; i++) {
// 		fd[i] = open_msr(i);
// 	}

//     int core_energy_units = read_msr(fd[0], AMD_MSR_PWR_UNIT);

// 	time_unit = (core_energy_units & AMD_TIME_UNIT_MASK) >> 16;
// 	energy_unit = (core_energy_units & AMD_ENERGY_UNIT_MASK) >> 8;
// 	power_unit = (core_energy_units & AMD_POWER_UNIT_MASK);
	
// 	time_unit_d = pow(0.5,(double)(time_unit));
// 	energy_unit_d = pow(0.5,(double)(energy_unit));
// 	power_unit_d = pow(0.5,(double)(power_unit));

// 	reset();
// }

void Rapl::reset() {
	prev_state = &state1;
	current_state = &state2;
	next_state = &state3;

    prev_state->pkg = (uint64_t*)malloc(sizeof(uint64_t)*total_cores/2);
	prev_state->pp0 = (uint64_t*)malloc(sizeof(uint64_t)*total_cores/2);

    current_state->pkg = (uint64_t*)malloc(sizeof(uint64_t)*total_cores/2);
	current_state->pp0 = (uint64_t*)malloc(sizeof(uint64_t)*total_cores/2);

    next_state->pkg = (uint64_t*)malloc(sizeof(uint64_t)*total_cores/2);
	next_state->pp0 = (uint64_t*)malloc(sizeof(uint64_t)*total_cores/2);

	// sample twice to fill current and previous
	sample();
	sample();

	// Initialize running_total
	running_total.pkg = 0;
	running_total.pp0 = 0;
	//gettimeofday(&(running_total.tsc), NULL);
}

void Rapl::free_state(){
	sample();

	// Initialize running_total
	running_total.pkg = 0;
	running_total.pp0 = 0;
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

uint64_t Rapl::read_msr(int fd, unsigned int msr_offset) {

	uint64_t data;

	if ( pread(fd, &data, sizeof data, msr_offset) != sizeof data ) {
		perror("rdmsr:pread");
		exit(127);
	}

	return data;
}

void Rapl::sample() {
	int package_raw;

	for (int i = 0; i < total_cores/2; i++) {
			try {
				package_raw = read_msr(fd[i], AMD_MSR_PACKAGE_ENERGY);
				next_state->pkg[i] = package_raw;
			} catch(...) {
				continue;
			}
		}
	running_total.pkg += energy_delta_pkg(current_state->pkg, next_state->pkg);
	
	// Rotate states
	rapl_state_t *pprev_state = prev_state;
	prev_state = current_state;
	current_state = next_state;
	next_state = pprev_state;
}

void Rapl::sample_core() {
	int core_energy_raw;

	
	core_energy_raw = read_msr(fd[current_core], AMD_MSR_CORE_ENERGY);

	next_state->pp0[current_core] = core_energy_raw;

	running_total.pp0 += energy_delta(current_state->pp0, next_state->pp0);
	
	// Rotate states
	rapl_state_t *pprev_state = prev_state;
	prev_state = current_state;
	current_state = next_state;
	next_state = pprev_state;
}


int Rapl::detect_packages(void) {

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

// double Rapl::time_delta(struct timeval *begin, struct timeval *end) {
//         return (end->tv_sec - begin->tv_sec)
//                 + ((end->tv_usec - begin->tv_usec)/1000000.0);
// }

// double Rapl::total_time() {
// 	return time_delta(&(running_total.tsc), &(current_state->tsc));
// }

// double Rapl::current_time() {
// 	return time_delta(&(prev_state->tsc), &(current_state->tsc));
// }

// double Rapl::pkg_current_power() {
// 	double t = time_delta(&(prev_state->tsc), &(current_state->tsc));
// 	return power_pkg(prev_state->pkg, current_state->pkg, t);
// }

// double Rapl::pp0_current_power() {
// 	double t = time_delta(&(prev_state->tsc), &(current_state->tsc));
// 	return power(prev_state->pp0, current_state->pp0, t);
// }

// double Rapl::power(uint64_t* before, uint64_t* after, double time_delta) {
// 	if (time_delta == 0.0f || time_delta == -0.0f) { return 0.0; }
// 	double energy = energy_unit_d * ((double) energy_delta(before,after));
// 	return energy / time_delta;
// }

// double Rapl::power_pkg(uint64_t* before, uint64_t* after, double time_delta) {
// 	if (time_delta == 0.0f || time_delta == -0.0f) { return 0.0; }
// 	double energy = energy_unit_d * ((double) (energy_delta_pkg(before,after)));
// 	return energy / time_delta;
// }

uint64_t Rapl::energy_delta(uint64_t* before, uint64_t* after) {
	uint64_t sum = 0;

    uint64_t max_int = ~((uint32_t) 0);
	for(int i = 0; i < total_cores/2; i++) {
        uint64_t tmp = 0;
		tmp = (after[i] - before[i]); //*10;
        if (before[i] > after[i]) {
		    tmp = (after[0] + (max_int - before[0]));// * 10;
	}
        sum += tmp;
	}
	return sum;
}

uint64_t Rapl::energy_delta_pkg(uint64_t* before, uint64_t* after) {
	uint64_t sum = 0;

    uint64_t max_int = ~((uint32_t) 0);
    uint64_t tmp = 0;

	tmp = (after[0] - before[0]); //*10;

    if (before[0] > after[0]) {
		    tmp = (after[0] + (max_int - before[0]));// * 10;
	}
    sum += tmp;
	return sum;
}

double Rapl::pkg_total_energy() {
	return energy_unit_d * ((double) running_total.pkg);
}

double Rapl::pp0_total_energy() {
	return energy_unit_d * ((double) running_total.pp0);
}