//
// Created by edward on 13.11.2019.
//

#ifndef LEDGER_APP_MINTER_HID_FRAME_APDU_H
#define LEDGER_APP_MINTER_HID_FRAME_APDU_H

#include "hid_config.h"
#include "hid.h"
#include "hid_frame.h"

namespace minter {

class hid_frame_apdu : public minter::hid_frame {
 public:
    hid_frame_apdu(const hid_dev &dev);
    bytes_data exchange(const APDU &apdu, uint16_t *statusCode = nullptr);

 private:
    bytes_data buf;
    size_t rn;
};

} // minter

#endif //LEDGER_APP_MINTER_HID_FRAME_APDU_H
