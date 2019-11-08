#### platform definitions
#DEFINES += ST31 gcc __IO=volatile
#
## default is not to display make commands
#log = $(if $(strip $(VERBOSE)),$1,@$1)
#
#CFLAGS   += -gdwarf-2  -gstrict-dwarf
#CFLAGS   += -O3 -Os
#CFLAGS   += -I/usr/include
#CFLAGS   += -fomit-frame-pointer
#CFLAGS   += -mcpu=cortex-m0 -mthumb
#CFLAGS   += -fno-common -mtune=cortex-m0 -mlittle-endian
#CFLAGS   += -std=gnu99 -Werror=int-to-pointer-cast -Wall -Wextra #-save-temps
#CFLAGS   += -fdata-sections -ffunction-sections -funsigned-char -fshort-enums
#CFLAGS   += -mno-unaligned-access
#CFLAGS   += -Wno-unused-parameter -Wno-duplicate-decl-specifier
#CFLAGS   += -fropi --target=armv6m-none-eabi
#CFLAGS   += -fno-jump-tables # avoid jump tables for switch to avoid problems with invalid PIC access
#
#AFLAGS += -ggdb2 -O3 -Os -mcpu=cortex-m0 -fno-common -mtune=cortex-m0
#
#LDFLAGS  += -gdwarf-2  -gstrict-dwarf
#LDFLAGS  += -O3 -Os
#LDFLAGS  += -I/usr/include
#LDFLAGS  += -fomit-frame-pointer
#LDFLAGS  += -Wall
#LDFLAGS  += -mcpu=cortex-m0 -mthumb
#LDFLAGS  += -fno-common -ffunction-sections -fdata-sections -fwhole-program -nostartfiles
#LDFLAGS  += -mno-unaligned-access
#ifndef SCRIPT_LD
#  SCRIPT_LD:=$(BOLOS_SDK)/script.ld
#endif
#LDFLAGS  += -T$(SCRIPT_LD)  -Wl,--gc-sections -Wl,-Map,debug/app.map,--cref
set(CMAKE_C_STANDARD 99)
add_definitions(-DST31 -Dgcc -D__IO=volatile)

set(CMAKE_ASM_FLAGS "-ggdb2 -O3 -Os -mcpu=cortex-m0 -fno-common -mtune=cortex-m0")

set(CMAKE_C_FLAGS "-gdwarf-2 -gstrict-dwarf")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Os")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m0 -mthumb")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common -mtune=cortex-m0 -mlittle-endian")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Werror=int-to-pointer-cast -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -funsigned-char -fshort-enums")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-unaligned-access")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-duplicate-decl-specifier")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fropi --target=armv6m-none-eabi")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-jump-tables")



