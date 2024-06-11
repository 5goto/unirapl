#include <iostream>
#include <fstream>
#include "interface.h"


Rapl* begin_energy_measurement() {
    Rapl *rapl = Rapl::get_instance();
    rapl->free_state();
    return rapl;
}


double complete_energy_measurement(Rapl* h) {
    h->sample();
    return h->total_energy();
}


int define_cpu() {
  std::ifstream cpuinfo("/proc/cpuinfo");
  if (!cpuinfo.is_open()) {
   std::cerr << "Error opening /proc/cpuinfo" << std::endl;
    return 3;
  }

  std::string line;
  while (getline(cpuinfo, line)) {
    if (line.find("vendor_id") != std::string::npos) {

      std::string vendor_id = line.substr(line.find(":") + 1);
      if (vendor_id == " GenuineIntel") {
        return 0; // intel
      } else if (vendor_id == " AuthenticAMD") {
        return 1; // amd 
      } else {
        return 3; // other
      }
    }
  }
  return 3;
}