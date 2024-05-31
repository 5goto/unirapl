#include "interface.h"

void init_rapl() {
    Rapl* instance = nullptr;
}

Rapl* begin_energy_measurement() {
    Rapl *rapl = Rapl::get_instance();
    rapl->free_state();
    return rapl;
}


double complete_energy_measurement(Rapl* h) {
    h->sample();
    return h->pkg_total_energy();
}

// Rapl* begin_energy_measurement_for_core(unsigned core_index) {
//     Rapl *rapl = new Rapl(core_index);
//     return rapl;
// }


// double complete_energy_measurement_for_core(Rapl* h, unsigned core_index) {
//     h->sample();
//     return h->pp0_total_energy();
// }
