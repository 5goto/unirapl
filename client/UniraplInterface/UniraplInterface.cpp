#include "UniraplInterface.h"

void startMeasure() {
    RaplSender* client = RaplSender::get_instance();

    auto res = client->request("start");
}

std::string endMeasure() {
    RaplSender* client = RaplSender::get_instance();

    return client->request("stop");
}

// PKG, SINGLE_CORE, ALL_CORES
void changeMode(std::string mode) {
    RaplSender* client = RaplSender::get_instance();

    auto res = client->request(mode);
}
