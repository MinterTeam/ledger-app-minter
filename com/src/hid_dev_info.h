//
// Created by edward on 13.11.2019.
//

#ifndef LEDGER_APP_MINTER_HID_DEV_INFO_H
#define LEDGER_APP_MINTER_HID_DEV_INFO_H

#include <hidapi.h>
#include "hid_config.h"
#include "hid_dev.h"

namespace minter {

class hid_dev_info {
 public:
    hid_dev_info(hid_device_info *ptr);

    [[nodiscard]] hid_dev open() const;
    const dev_info_t &get() const;
 private:
    dev_info_t m_info;

};

} // minter


#endif //LEDGER_APP_MINTER_HID_DEV_INFO_H
