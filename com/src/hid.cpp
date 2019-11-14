//
// Created by edward on 13.11.2019.
//

#include "hid.h"
#include "hid_config.h"

minter::hid::hid() {
    hid_init();
}
minter::hid::~hid() {
    hid_exit();
}

std::vector<minter::hid_dev_info> minter::hid::enumerate_devices(uint16_t vendorId, uint16_t productId) {
    std::vector<hid_dev_info> infos;
    hid_device_info *current = hid_enumerate(vendorId, productId);
    while (current != nullptr) {
        infos.push_back(hid_dev_info(current));
        current = current->next;
    }

    return infos;
}
