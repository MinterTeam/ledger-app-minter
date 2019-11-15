#*******************************************************************************
#   Ledger App
#   (c) 2017 Ledger
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#*******************************************************************************
SCP_PRIVKEY=520ad34420c03953279c40a1e973ec43f81dbef79e469c031ea57315a4cfa7d4
SCP_PUBKEY=047fbc17bedec5cd055715b41c8d004a73c5b9c8ecf6a2f7b74a92c931b5c274a67a8a215cdb5690a91481f4b4cef3be8cbcb1ea050029f07082b5c9dbe052510d

BOLOS_SDK := $(PWD)/libs/sdk
ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif
include $(BOLOS_SDK)/Makefile.defines

TARGET_ID_NANOX=0x33000004
TARGET_NAME_NANOX=TARGET_NANOX
TARGET_ID_NANOS=0x31100004
TARGET_ID_NANOS=TARGET_NANOS

#TARGET_ID=$(TARGET_ID_NANOX)
#TARGET_NAME=$(TARGET_NAME_NANOX)

#########
#  App  #
#########

APPNAME    = Minter
ICONNAME   = nanos_app_minter.gif
APPVERSION = 0.1.0

#Target list
#0x31100002 on Nano S with firmware <= 1.3.1
#0x31100003 on Nano S with firmware 1.4.x
#0x31100004 on Nano S with firmware 1.5.x
#0x31000002 on Blue with firmware <= 2.0
#0x31000004 on Blue with firmware 2.1.x
#0x31010004 on Blue v2 with firmware 2.1.x
#0x33000004 on Nano X with firmware 1.x

# The --path argument here restricts which BIP32 paths the app is allowed to derive.
APP_LOAD_PARAMS = --appFlags 0x40 --path "44'/60'" --curve secp256k1 $(COMMON_LOAD_PARAMS)
APP_SOURCE_PATH = src
SDK_SOURCE_PATH = lib_stusb lib_stusb_impl

all: default

# This target will initialize the device with the integration testing mnemonic
dev_init:
	@echo "Initializing device with test mnemonic! WARNING TAKES 2 MINUTES AND REQUIRES RECOVERY MODE"
	@python -m ledgerblue.hostOnboard --apdu --id 0 --prefix "" --passphrase "" --pin 5555 --words "december wedding engage learn plate lion phone lemon hill grocery effort dismiss"

# This target will setup a custom developer certificate
dev_ca:
	python3 -m ledgerblue.setupCustomCA --targetId $(TARGET_ID) --public $(SCP_PUBKEY) --name dev

dev_ca_delete:
	python3 -m ledgerblue.resetCustomCA --targetId $(TARGET_ID)

load: all
	python3 -m ledgerblue.loadApp --apdu $(APP_LOAD_PARAMS)

delete:
	python3 -m ledgerblue.deleteApp $(COMMON_DELETE_PARAMS)

############
# Platform #
############

DEFINES += OS_IO_SEPROXYHAL IO_SEPROXYHAL_BUFFER_SIZE_B=128
DEFINES += HAVE_BAGL HAVE_SPRINTF
DEFINES += HAVE_IO_USB HAVE_L4_USBLIB IO_USB_MAX_ENDPOINTS=7 IO_HID_EP_LENGTH=64 HAVE_USB_APDU
DEFINES += APPVERSION=\"$(APPVERSION)\"
# DEBUG printf
DEFINES += HAVE_SPRINTF HAVE_PRINTF PRINTF=screen_printf

ifeq ($(TESTING),1)
	DEFINES += TESTING=1
endif

##############
#  Compiler  #
##############

current_dir = $(shell pwd)

CC := $(CLANGPATH)clang
CFLAGS += -O0 -Os -Wno-sign-compare -Wno-typedef-redefinition -Wno-int-conversion -I$(current_dir)/libs/secp256k1/include

AS := $(GCCPATH)arm-none-eabi-gcc
LD := $(GCCPATH)arm-none-eabi-gcc
LDFLAGS += -O0 -Os -I$(current_dir)/libs/secp256k1/include
LDLIBS += -lm -lgcc -lc

##-specs=nosys.specs

##################
#  Dependencies  #
##################

# import rules to compile glyphs
include $(BOLOS_SDK)/Makefile.glyphs
# import generic rules from the sdk
include $(BOLOS_SDK)/Makefile.rules

dep/%.d: %.c Makefile

listvariants:
	@echo VARIANTS COIN bip
