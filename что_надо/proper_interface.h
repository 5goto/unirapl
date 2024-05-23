#pragma once

typedef unsigned long long energy_handle;
energy_handle begin_energy_measurement();
double complete_energy_measurement(energy_handle h);
energy_handle begin_energy_measurement_for_core(unsigned core_index);
double complete_energy_measurement_for_core(energy_handle h, unsigned core_index);

