#include <iostream>
#include <fstream>


int main() {
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
        return 1; // intel
      } else if (vendor_id == " AuthenticAMD") {
        return 2; // amd 
      } else {
        return 3; // other
      }
    }
  }
  return 3;
}