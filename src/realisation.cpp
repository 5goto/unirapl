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
