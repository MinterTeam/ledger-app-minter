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
#SCP_PRIVKEY=6d0c5eb521e8c205e034c7e5c65fd7785eaa8ac8a60edcffe1eb238acf6541fa
#SCP_PUBKEY=04c15facb4e2aae57eac019dcc8806152a3104da9f592fd1a1b01623c3937a8c49f428aa64bc17d8d58508410f90e9ef11461de3d61b16a819282544eeb82aa039

BOLOS_SDK := $(PWD)/libs/sdk
ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif
include $(BOLOS_SDK)/Makefile.defines

TARGET_ID_NANOX=0x33000004
TARGET_NAME_NANOX=TARGET_NANOX
TARGET_ID_NANOS=0x31100004
TARGET_NAME_NANOS=TARGET_NANOS

#TARGET_ID=$(TARGET_ID_NANOX)
#TARGET_NAME=$(TARGET_NAME_NANOX)

#########
#  App  #
#########

APPNAME    = Minter
ICONNAME   = nanos_app_minter.gif
APPVERSION = $(shell cat version | tr -d "\n")

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
SDK_SOURCE_PATH = lib_stusb lib_stusb_impl lib_u2f
ifeq ($(TARGET_NAME),TARGET_NANOX)
	SDK_SOURCE_PATH  += lib_blewbxx lib_blewbxx_impl
	SDK_SOURCE_PATH  += lib_ux
endif

all: default

ifeq ($(TARGET_NAME),TARGET_NANOX)
	DEFINES += HAVE_BLE BLE_COMMAND_TIMEOUT_MS=2000 HAVE_BLE_APDU
endif

# This target will initialize the device with the integration testing mnemonic
dev_init:
	@echo "Initializing device with test mnemonic! WARNING TAKES 2 MINUTES AND REQUIRES RECOVERY MODE"
	@python3 -m ledgerblue.hostOnboard --apdu --id 0 --prefix "" --passphrase "" --pin 5555 --words "december wedding engage learn plate lion phone lemon hill grocery effort dismiss"

# This target will setup a custom developer certificate
dev_ca:
	python3 -m ledgerblue.setupCustomCA --targetId $(TARGET_ID) --public $(SCP_PUBKEY) --name dev

dev_ca_delete:
	python3 -m ledgerblue.resetCustomCA --targetId $(TARGET_ID)

load: all
	python3 -m ledgerblue.loadApp $(APP_LOAD_PARAMS)

delete:
	python3 -m ledgerblue.deleteApp $(COMMON_DELETE_PARAMS)

############
# Platform #
############

ifeq ($(TARGET_NAME),TARGET_NANOX)
DEFINES += OS_IO_SEPROXYHAL IO_SEPROXYHAL_BUFFER_SIZE_B=300
DEFINES += HAVE_GLO096
DEFINES += HAVE_BAGL BAGL_WIDTH=128 BAGL_HEIGHT=64
DEFINES += HAVE_BAGL_ELLIPSIS # long label truncation feature
DEFINES += HAVE_BAGL_FONT_OPEN_SANS_REGULAR_11PX
DEFINES += HAVE_BAGL_FONT_OPEN_SANS_EXTRABOLD_11PX
DEFINES += HAVE_BAGL_FONT_OPEN_SANS_LIGHT_16PX
DEFINES += HAVE_UX_FLOW
else
DEFINES += OS_IO_SEPROXYHAL IO_SEPROXYHAL_BUFFER_SIZE_B=128
endif

DEFINES += HAVE_BAGL HAVE_SPRINTF
DEFINES += HAVE_IO_USB HAVE_L4_USBLIB IO_USB_MAX_ENDPOINTS=7 IO_HID_EP_LENGTH=64 HAVE_USB_APDU
## USB U2F04c15facb4e2aae57eac019dcc8806152a3104da9f592fd1a1b01623c3937a8c49f428aa64bc17d8d58508410f90e9ef11461de3d61b16a819282544eeb82aa039
DEFINES += HAVE_U2F HAVE_IO_U2F U2F_PROXY_MAGIC=\"ADA\" USB_SEGMENT_SIZE=64
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
CFLAGS += -m32 -O0 -Os -Wno-sign-compare -Wno-typedef-redefinition -Wno-int-conversion -I$(current_dir)/libs/secp256k1/include

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
