//
// Created by edward on 13.11.2019.
//
#include <fmt/format.h>
#include "hid_dev.h"
minter::hid_dev::hid_dev(uint16_t vendorId, uint16_t productId, const wchar_t *serial) {
    auto *h = hid_open(vendorId, productId, nullptr);
    m_dev = dev_handle_t(h);
}
bool minter::hid_dev::valid() {
    return m_dev != nullptr && m_dev.get() != nullptr;
}

const minter::dev_handle_t &minter::hid_dev::get() const {
    return m_dev;
}

std::string minter::hid_dev::getError() {
    std::string err;
    get_dev_error(*this, err);
    return err;
}

size_t minter::hid_dev::write(const uint8_t *data, size_t len) const {
    return hid_write(m_dev.get(), data, len);
}

size_t minter::hid_dev::write(const bytes_data &data) const {
    return write(data.cdata(), data.size());
}

size_t minter::hid_dev::read(bytes_data &out, size_t readLen) const {
    auto res = hid_read(m_dev.get(), out.data(), readLen);
    auto err = hid_error(m_dev.get());
    if (err) {
        std::cerr << "Unable to read " << readLen << " bytes: " << err << std::endl;
    }
    return res;
}

size_t minter::hid_dev::read_back(bytes_data &out, size_t seq, size_t readLen) const {
    uint8_t buf[readLen];
    auto res = hid_read(m_dev.get(), buf, readLen);
    auto err = hid_error(m_dev.get());
    if (err) {
        std::stringstream ss;
        ss << err;
        ML_ERR("Unable to read {0} bytes: {1}", readLen, ss.str());
        return 0;
    }

    size_t pos = seq * readLen;
    if(out.size() <= pos) {
        out.resize( out.size() + pos);
    }
    out.write(seq * readLen, buf, readLen);
    return readLen;
}

size_t minter::hid_dev::read(uint8_t *out, size_t readLen) const {
    if (!out) {
        throw std::runtime_error("Output variable is null");
    }
    return hid_read(m_dev.get(), out, readLen);
}

void minter::hid_dev::get_dev_error(const minter::hid_dev &dev, std::string &out) {
    if (dev.get() != nullptr && dev.get().get() != nullptr) {
        const wchar_t *err = hid_error(dev.get().get());
        if (err != nullptr) {
            std::wstring tmp(err);
            out = std::string(tmp.begin(), tmp.end());
            return;
        }
    }

    std::stringstream ss;
    const std::string errv(std::strerror(errno));
    ss << "[" << errno << "] " << errv;
    out = ss.str();
}
