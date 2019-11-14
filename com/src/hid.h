//
// Created by edward on 13.11.2019.
//

#ifndef LEDGER_APP_MINTER_HID_H
#define LEDGER_APP_MINTER_HID_H

#include <vector>
#include <memory>

#include "hid_config.h"
#include "hid_dev.h"
#include "hid_dev_info.h"

namespace minter {

class hid {
 public:
    hid();
    ~hid();

    std::vector<hid_dev_info> enumerate_devices(uint16_t vendorId, uint16_t productId);
};

} // minter

#endif //LEDGER_APP_MINTER_HID_H
