# ledger-app-minter

This is the official Minter wallet app for the Ledger Nano S.

# Dev Debian Requirements Guide
- apt install gcc-multilib clang libusb-dev libudev-dev usbutils python python-pip
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
# Nano X
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2c97", ATTRS{idProduct}=="0004", MODE="0660", TAG+="uaccess", TAG+="udev-acl" GROUP="users"

KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="users"
```

Then apply config:
```bash
sudo udevadm control --reload
```

# Build
```bash
make -j4
```

# Deploy
```bash
make load
```

For more information, read the docs

https://ledger.readthedocs.io/en/latest/userspace/getting_started.html


