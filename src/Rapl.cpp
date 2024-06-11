#include "Rapl.h"
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
#include "RaplConfig.h"


Rapl* Rapl::instance = nullptr; // singleton

Rapl::Rapl() {
	detect_packages();
	
	RaplConfig* conf = new RaplConfig;
	get_architecture(conf);

    // if(core > total_cores/2) {
	// 	unsigned core_number;

	// 	printf("Avialible cors: [%d:%d]", 0, total_cores/2 - 1);
	// 	do {
	// 		printf("Avialible cors: [%d:%d]", 0, total_cores/2 - 1);
	// 		std::cin >> core_number;
	// 	} while (core_number < 0 || core_number >= total_cores / 2);
		
	// 	current_core = core_number;
	// } 
    

	fd = open_msr(current_core);
    int core_energy_units = read_msr(fd, config_.MSR_PWR_UNIT);
	
	
	time_unit = (core_energy_units & config_.TIME_UNIT_MASK) >> 16;
	energy_unit = (core_energy_units & config_.ENERGY_UNIT_MASK) >> 8;
	power_unit = (core_energy_units & config_.POWER_UNIT_MASK);
	

	time_unit_d = std::scalbn(0.5, -time_unit + 1);
	energy_unit_d = std::scalbn(0.5, -energy_unit + 1);
	power_unit_d = std::scalbn(0.5, -power_unit + 1);

	reset();
}

Rapl::~Rapl() {
	free(prev_state);
	free(current_state);
	free(next_state);
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
			exit(126);
		}
	}

	return fd;
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
		fclose(fff);

		if (package_map[package]==-1) {
			total_packages++;
			package_map[package]=i;
		}

	}
	total_cores=i;
	return 0;
}


uint64_t Rapl::energy_delta(uint64_t* before, uint64_t* after) {
	uint64_t sum = 0; // optimize

    uint64_t max_int = ~((uint32_t) 0);
    uint64_t tmp = 0;

	tmp = (after[0] - before[0]);

    if (before[0] > after[0]) {
		    tmp = (after[0] + (max_int - before[0]));// * 10;
	}
    sum += tmp;
	return sum;
}

void Rapl::sample() {
	int package_raw;


	package_raw = read_msr(fd, config_.MSR_PACKAGE_ENERGY);
	next_state->pkg[0] = package_raw;

	running_total.pkg += energy_delta(current_state->pkg, next_state->pkg);
	
	// Rotate states
	rapl_state_t *pprev_state = prev_state;
	prev_state = current_state;
	current_state = next_state;
	next_state = pprev_state;
}

uint64_t Rapl::read_msr(int fd, unsigned int msr_offset) {

	uint64_t data;

	if ( pread(fd, &data, sizeof data, msr_offset) != sizeof data ) {
		perror("Ошибка чтения MSR");
		exit(126);
	}

	return data;
}


double Rapl::total_energy() {
	return energy_unit_d * ((double) *running_total.pkg);
}


void Rapl::reset() {
	prev_state = &state1;
	current_state = &state2;
	next_state = &state3;

    prev_state->pkg = (uint64_t*)malloc(sizeof(uint64_t)*total_cores/2);
    current_state->pkg = (uint64_t*)malloc(sizeof(uint64_t)*total_cores/2);
    next_state->pkg = (uint64_t*)malloc(sizeof(uint64_t)*total_cores/2);

	sample();
	sample();

	running_total.pkg = 0;
}

void Rapl::free_state() {
	sample();
	running_total.pkg = 0;
}

void Rapl::get_architecture(RaplConfig* conf) {
	architecture = conf->get_string("architecture");
	//if (architecture != "AMD" && architecture != "INTEL")
}

void Rapl::get_target(RaplConfig* conf) {
	target = conf->get_string("target");
}

void Rapl::get_core(RaplConfig* conf) {
	current_core = conf->get_number("core");
}

void Rapl::get_msr(RaplConfig* conf) {
	config_.POWER_UNIT_MASK = conf->get_msr(architecture + "_POWER_UNIT_MASK");
	config_.TIME_UNIT_MASK = conf->get_msr(architecture + "_TIME_UNIT_MASK");
	config_.ENERGY_UNIT_MASK = conf->get_msr(architecture + "_ENERGY_UNIT_MASK");
	config_.MSR_PWR_UNIT = conf->get_msr(architecture + "_MSR_PWR_UNIT");
	config_.MSR_PACKAGE_ENERGY = conf->get_msr(architecture + "_MSR_PACKAGE_ENERGY");
	config_.MSR_CORE_ENERGY = conf->get_msr(architecture + "_MSR_CORE_ENERGY");
}

// void Rapl::init_amd() {
//     config_.MSR_PWR_UNIT = 0xC0010299;
//     config_.TIME_UNIT_MASK = 0xF0000;
//     config_.ENERGY_UNIT_MASK = 0x1F00;
//     config_.POWER_UNIT_MASK = 0xF;
//     config_.MSR_PACKAGE_ENERGY = 0xC001029B;
//     config_.MSR_CORE_ENERGY = 0xC001029A;
// }

// void Rapl::init_intel() {
//     config_.MSR_PWR_UNIT = 0x606;
//     config_.TIME_UNIT_MASK = 0xf;
//     config_.ENERGY_UNIT_MASK = 0x1f;
//     config_.POWER_UNIT_MASK = 0xf;
//     config_.MSR_PACKAGE_ENERGY = 0x611;
//     config_.MSR_CORE_ENERGY = 0x639;
// }