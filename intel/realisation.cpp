#include "interface.h"



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
//     Rapl *rapl = Rapl::get_instance_core(core_index);
//     rapl->free_state();
//     return rapl;
// }


// double complete_energy_measurement_for_core(Rapl* h) {
//     h->sample_core();
//     return h->pp0_total_energy();
// }

