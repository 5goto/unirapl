#include "Rapl.h"


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


uint64_t Rapl::energy_delta(uint64_t before, uint64_t after) {
	uint64_t sum = 0; // optimize

    uint64_t max_int = ~((uint32_t) 0);
    uint64_t tmp = 0;

	tmp = (after - before);

    if (before > after) {
		    tmp = (after + (max_int - before));
	}
    sum += tmp;
	return sum;
}

uint64_t Rapl::read_msr(int fd, unsigned int msr_offset) {

	uint64_t data;

	if ( pread(fd, &data, sizeof data, msr_offset) != sizeof data ) {
		perror("Ошибка чтения MSR");
		exit(126);
	}
	return data;
}


std::vector<double> Rapl::total_energy() {
    std::vector<double> result;
    result.push_back(energy_unit_d * ((double) measure_result));
	return result;
}


void Rapl::start_measure() {
	energy_acc_first = read_msr(fd, config_.MSR_PACKAGE_ENERGY);
	energy_acc_second = 0;
}

void Rapl::complete_measure() {
	energy_acc_second = read_msr(fd, config_.MSR_PACKAGE_ENERGY);
	measure_result = energy_delta(energy_acc_first, energy_acc_second);
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

void Rapl::detect_cpu_topology() {
  // Определение количества ядер
  topology.num_cores = sysconf(_SC_NPROCESSORS_ONLN);

  // Определение количества пакетов
  int package_ids[MAX_PACKAGES] = { -1 };
  int num_packages = 0;
  for (int cpu_id = 0; cpu_id < topology.num_cores; ++cpu_id) {
    std::string filename = "/sys/devices/system/cpu/cpu" + std::to_string(cpu_id) + "/topology/physical_package_id";
    std::ifstream file(filename);

    if (file.is_open()) {
      int package_id;
      file >> package_id;
      file.close();

      if (package_ids[package_id] == -1) {
        num_packages++;
        package_ids[package_id] = cpu_id;
      }
    } else {
      std::cerr << "Ошибка открытия файла: " << filename << std::endl;
    }
  }
  topology.num_packages = num_packages;
}
