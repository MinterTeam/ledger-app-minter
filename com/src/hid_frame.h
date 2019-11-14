//
// Created by edward on 13.11.2019.
//

#ifndef LEDGER_APP_MINTER_HID_FRAME_H
#define LEDGER_APP_MINTER_HID_FRAME_H

#include <fmt/format.h>
#include "hid_config.h"
#include "hid.h"

namespace minter {

class hid_frame {
 public:
    hid_frame(const hid_dev &dev);

    size_t read(bytes_data &out);
    size_t write(const APDU &apdu);
    void reset();

    const hid_dev &io;
    uint16_t seq;
    size_t offset = 0;
    bytes_buffer m_buffer;
};

} // minter

#endif //LEDGER_APP_MINTER_HID_FRAME_H
