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
#include <fstream>
#include <cstdlib>


Rapl* Rapl::instance = nullptr; // singleton

// Rapl::Rapl() {
// 	detect_packages();
	
// 	conf = new RaplConfig;
// 	conf->parseConfig();

// 	get_architecture(conf);
// 	get_target(conf);
// 	get_core(conf);
//     get_msr(conf);

// 	fd = open_msr(current_core);
//     int core_energy_units = read_msr(fd, config_.MSR_PWR_UNIT);
	
	
// 	time_unit = (core_energy_units & config_.TIME_UNIT_MASK) >> 16;
// 	energy_unit = (core_energy_units & config_.ENERGY_UNIT_MASK) >> 8;
// 	power_unit = (core_energy_units & config_.POWER_UNIT_MASK);
// 	std::cout << time_unit << " " << energy_unit << " " << power_unit << std::endl;

// 	time_unit_d = std::scalbn(0.5, -time_unit + 1);
// 	energy_unit_d = std::scalbn(0.5, -energy_unit + 1);
// 	power_unit_d = std::scalbn(0.5, -power_unit + 1);

// 	reset();
// }

Rapl::Rapl() {
	detect_packages();
  
	conf = new RaplConfig;
	conf->parseConfig();

	get_architecture(conf);
	get_target(conf);
	get_core(conf);
    get_msr(conf);

	fd = open_msr(current_core);
    uint64_t core_energy_units = read_msr(fd, config_.MSR_PWR_UNIT);

	if(architecture == "INTEL") {
		power_unit = (double) (core_energy_units & config_.POWER_UNIT_MASK);
		energy_unit = (double) ((core_energy_units >> 8) & config_.ENERGY_UNIT_MASK);
		time_unit = (double) ((core_energy_units >> 16) & config_.TIME_UNIT_MASK);
		printf("Time_unit:%f, Energy_unit: %f, Power_unit: %f\n", time_unit, energy_unit, power_unit);
	} else {
		time_unit = (core_energy_units & config_.TIME_UNIT_MASK) >> 16;
		energy_unit = (core_energy_units & config_.ENERGY_UNIT_MASK) >> 8;
		power_unit = (core_energy_units & config_.POWER_UNIT_MASK);
		printf("Time_unit:%f, Energy_unit: %f, Power_unit: %f\n", time_unit, energy_unit, power_unit);
	}

  	time_unit_d = std::scalbn(0.5, -time_unit + 1);
  	energy_unit_d = std::scalbn(0.5, -energy_unit + 1);
  	power_unit_d = std::scalbn(0.5, -power_unit + 1);
	printf("Time_unit:%g, Energy_unit: %g, Power_unit: %g\n", time_unit_d, energy_unit_d, power_unit_d);

	reset();
}

Rapl::~Rapl() {
	free(prev_state);
	free(current_state);
	free(next_state);
	delete conf;
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

	package_raw = (target == "PKG") ? read_msr(fd, config_.MSR_PACKAGE_ENERGY) : read_msr(fd, config_.MSR_CORE_ENERGY);

	next_state->pkg[0] = package_raw;

	all += energy_delta(current_state->pkg, next_state->pkg);
	
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
	return energy_unit_d * ((double) all);
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

	all = 0;
}

void Rapl::free_state() {
	sample();
	all = 0;
}

void Rapl::get_architecture(RaplConfig* conf) {
	bool auto_detect = conf->get_bool("auto_detect_architecture");
	if(auto_detect) {
		std::string arch = get_arch_by_cpuinfo();
		if(arch == "OTHER") {
			architecture = conf->get_string("architecture");
		} else {
			architecture = arch;
		}
	} else {
		architecture = conf->get_string("architecture");
	}
}

void Rapl::get_target(RaplConfig* conf) {
	target = conf->get_string("target");
}

void Rapl::get_core(RaplConfig* conf) {
	if(target != "CORE") {
		current_core = conf->get_number("core");
		if(current_core > total_cores/2) {
			printf("Avialible cors: [%d:%d]", 0, total_cores/2 - 1);
			printf("Set default core: 0");
			current_core = 0;
		} 
	} else {
		current_core = 0;
	}
}

void Rapl::get_msr(RaplConfig* conf) {
	config_.POWER_UNIT_MASK = conf->get_msr(architecture + "_POWER_UNIT_MASK");
	config_.TIME_UNIT_MASK = conf->get_msr(architecture + "_TIME_UNIT_MASK");
	config_.ENERGY_UNIT_MASK = conf->get_msr(architecture + "_ENERGY_UNIT_MASK");
	config_.MSR_PWR_UNIT = conf->get_msr(architecture + "_MSR_PWR_UNIT");
	config_.MSR_PACKAGE_ENERGY = conf->get_msr(architecture + "_MSR_PACKAGE_ENERGY");
	config_.MSR_CORE_ENERGY = conf->get_msr(architecture + "_MSR_CORE_ENERGY");
}

std::string Rapl::get_arch_by_cpuinfo() {
  std::ifstream cpuinfo("/proc/cpuinfo");   

  if (!cpuinfo.is_open()) {
   std::cerr << "Error opening /proc/cpuinfo" << std::endl;
    return "OTHER";
  }

  std::string line;
  while (getline(cpuinfo, line)) {
    if (line.find("vendor_id") != std::string::npos) {

      std::string vendor_id = line.substr(line.find(":") + 1);
      if (vendor_id == " GenuineIntel") {
        return "INTEL"; // intel
      } else if (vendor_id == " AuthenticAMD") {
        return "AMD"; // amd 
      } else {
        return "OTHER"; // other
      }
    }
  }
  return "OTHER";
}