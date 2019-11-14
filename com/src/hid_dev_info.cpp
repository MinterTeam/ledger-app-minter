//
// Created by edward on 13.11.2019.
//

#include "hid_dev_info.h"

minter::hid_dev_info::hid_dev_info(hid_device_info *ptr) : m_info(dev_info_t(ptr)) { }

minter::hid_dev minter::hid_dev_info::open() const {
    return minter::hid_dev(m_info->vendor_id, m_info->product_id, m_info->serial_number);
}
const minter::dev_info_t &minter::hid_dev_info::get() const {
    return m_info;
}
