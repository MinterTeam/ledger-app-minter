# ledger-app-minter

This is the official Minter wallet app for the Ledger Nano S.

# DISCLAIMER
This wallet is "work in progress" and development is unfinished yet. So use it on your own risk.

# Dev Debian Requirements Guide
- apt install gcc-multilib clang libusb-dev libusb-1.0-0-dev libudev-dev usbutils python python-pip python-dev
- pip install image
- pip install ledgerblue

# udev permissions
Create file **/etc/udev/rules.d/99-ledger.rules**

```
# Ledger devices

# Blue
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2c97", ATTRS{idProduct}=="0000", MODE="0660", TAG+="uaccess", TAG+="udev-acl" GROUP="users"
# Nano S
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2c97", ATTRS{idProduct}=="0001", MODE="0660", TAG+="uaccess", TAG+="udev-acl" GROUP="users"
# Nano S v1.6+ system
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2c97", ATTRS{idProduct}=="1011", MODE="0660", TAG+="uaccess", TAG+="udev-acl" GROUP="users"
# Nano S v1.6+ app
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2c97", ATTRS{idProduct}=="1005", MODE="0660", TAG+="uaccess", TAG+="udev-acl" GROUP="users"
# Nano X
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2c97", ATTRS{idProduct}=="0004", MODE="0660", TAG+="uaccess", TAG+="udev-acl" GROUP="users"

KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="users"
```

Then apply config:

```bash
sudo udevadm control --reload
```

If still no access - just reboot computer.

# Install pre-built app (firmware 1.6.1 or newest). Works for macOS and Linux

1. To install an app, you need to install **python3** and **pip3** using your favourite package manager
2. Next install 2 python dependencies:
    - `pip3 install image`
    - `pip3 install ledgerblue`

3. Connect your ledger via usb
4. Recover from existed 12 words mnemonic (Minter supports only 12 words)
5. Enter PIN-code every time device asking
6. Install app

```bash
git clone https://github.com/MinterTeam/ledger-app-minter
cd dist
chmod +x install.sh
./install.sh
```

As this app is not verified by Ledger and don't have valid certificates, device will warn you about non-genuine
application et cetera et cetera. Just ignore it and approve all requests.

# Build

1. First of all - Linux is only one system that can build app without rocket science knowledge.
2. Create directory `/opt/bolos-devenv`
3. Download old version gcc-multilib for arm
   devices: [https://launchpad.net/gcc-arm-embedded/+milestone/5-2016-q1-update](https://launchpad.net/gcc-arm-embedded/+milestone/5-2016-q1-update)
   and unarchive all contains to `/opt/bolos-devenv/gcc-arm-none-eabi-5_3-2016q1`. `bin` and other directories must be
   in root of created directory.
4. Download old version of
   clang [http://releases.llvm.org/download.html#4.0.0](http://releases.llvm.org/download.html#4.0.0) and do the same
   for directory: `/opt/bolos-devenv/clang-arm-fropi`
5. Add environment variable `BOLOS_ENV=/opt/bolos-devenv` to `~/.bashrc` or wherever you wish and publish it
   with `source ~/.bashrc`
6. Now you can just build

```bash
make -j4
```

# Deploy

```bash
make load
```

### For more information: read the docs

https://ledger.readthedocs.io/en/latest/userspace/getting_started.html


