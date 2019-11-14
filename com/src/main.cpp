//
// Created by edward on 08.11.2019.
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <hidapi.h>
#include <utility>
#include <vector>
#include <memory>
#include <minter/address.h>
#include <fmt/format.h>
#include <exception>

#include <boost/program_options.hpp>

#include <toolboxpp.hpp>
#include "hid_config.h"
#include "hid.h"
#include "hid_frame_apdu.h"

constexpr const uint8_t CMD_GET_PUBLIC_KEY = 0x01u << 1u;
constexpr const uint8_t CMD_SIGN_TX = 0x01u << 2u;
constexpr const uint8_t CMD_GET_PRIVATE_KEY = 0x01u << 4u;

// by default, show address instead pubkey
const uint8_t P2_DISPLAY_ADDRESS = 0x00;
const uint8_t P2_DISPLAY_PUBKEY = 0x01;
// by default, show tx hash instead of sign it
const uint8_t P2_DISPLAY_HASH = 0x00;
const uint8_t P2_SIGN_HASH = 0x01;

class NanoS {
 public:
    explicit NanoS(minter::hid_frame_apdu frame) : device(std::move(frame)) { }

    bytes_data exchange(const minter::APDU &apdu, uint16_t *resCode = nullptr) {
        return device.exchange(apdu, resCode);
    }
 private:
    minter::hid_frame_apdu device;
};

int main(int argc, char **argv) {

    minter::hid hid;
    auto devs = hid.enumerate_devices(LEDGER_VID, NANOS_PID);
    if (devs.empty()) {
        std::cerr << "Please, connect your Nano S to computer" << std::endl;
        return 1;
    } else if (devs.size() > 1) {
        std::cerr << "Did you opened Minter wallet?" << std::endl;
        return 1;
    }

    auto dev_info = devs.begin();

    auto dev = dev_info->open();
    if (!dev.valid()) {
        std::cerr << "Unable to open device ( " << dev_info->get()->path << " ): " << dev.getError() << std::endl;
        if (errno == EACCES) {
            std::cerr << "Try to run this executable with sudo or grant " << dev_info->get()->path
                      << " RW permissions to your user" << std::endl;
        }
        return 255;
    }

    minter::hid_frame_apdu apdu_frame(dev);

//    uint8_t cmd = CMD_GET_PUBLIC_KEY;
    uint8_t cmd = CMD_SIGN_TX;

    minter::APDU apdu;
    if (cmd == CMD_GET_PUBLIC_KEY) {
        apdu = {
            0xe0,
            cmd,
            0,
            0,
            4,
            {0, 0, 0, 0}
        };
    } else if (cmd == CMD_SIGN_TX) {
        bytes_data unsignedHash(34);
        unsignedHash.write(0, (uint32_t) 0);
        unsignedHash.write(4, "067ea2037c3f14e7fb1affaec93940bc5af500144f13274e131c9ae78cbbd4e1");
        apdu = {
            0xe0,
            cmd,
            0,
            0,
            4 + 32,
            {}
        };
        memcpy(apdu.payload, unsignedHash.cdata(), unsignedHash.size());
        assert(apdu.payload[0] == unsignedHash[0]);
        assert(apdu.payload[unsignedHash.size() - 1] == unsignedHash[unsignedHash.size() - 1]);
    }

    NanoS device(apdu_frame);
    uint16_t status = 0;
    auto response = device.exchange(apdu, &status);
//    bytes_data response(4);

    ML_LOG("Status code: 0x{0:X}", status);
    if (cmd == CMD_GET_PUBLIC_KEY) {
        minter::pubkey_t pubkey(response.take_range(0, 65));
        minter::address_t address(response.take_range(65, 65 + 20));
//        minter::address_t address(response.take_range(0, 20));
        std::cout << "Result: \n";
        std::cout << "Valid address: Mxbf5c2fec34cfe73e7178b3ab96deaf9ca6d9a592\n";
        std::cout << "Input address: " << address.to_string() << std::endl;
    } else if (cmd == CMD_GET_PRIVATE_KEY) {
        minter::privkey_t privey(response.get());
        std::cout << "Priv " << privey.to_hex() << std::endl;
    } else if (cmd == CMD_SIGN_TX) {
        std::cout << "Result:\n";
        std::cout << "Valid sign:    "
                  << "5f3346bef4bc527e177a8d74ae6b9a746a20949e80771fa3538caae4cfc45579790b23eef2537fd262079e918ed4625eebdb45600fb3e8d8340d59f655bda6a51b"
                  << std::endl;
        std::cout << "Response data: " << response.to_hex() << std::endl;

    }

    return 0;
}

