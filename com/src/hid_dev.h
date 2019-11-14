//
// Created by edward on 13.11.2019.
//

#ifndef LEDGER_APP_MINTER_HID_DEV_H
#define LEDGER_APP_MINTER_HID_DEV_H

#include <vector>
#include <string>
#include <cerrno>
#include <toolboxpp.hpp>
#include "hid_config.h"

namespace minter {

class hid_dev {
 public:
    hid_dev(uint16_t vendorId, uint16_t productId, const wchar_t *serial);

    bool valid();
    std::string getError();
    [[nodiscard]] const dev_handle_t &get() const;

    size_t write(const uint8_t *data, size_t len) const;
    size_t write(const bytes_data &data) const;
    size_t read(bytes_data &out, size_t readLen) const;
    size_t read_back(bytes_data &out, size_t seq, size_t readLen) const;
    size_t read(uint8_t *out, size_t readLen) const;

 private:
    void get_dev_error(const hid_dev &dev, std::string &out);

    dev_handle_t m_dev;
};

} // minter

#endif //LEDGER_APP_MINTER_HID_DEV_H
