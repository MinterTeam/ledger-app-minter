project(ledget-app-sia
        LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.13)

include(modules/utils.cmake)
SET(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
SET(CMAKE_CROSSCOMPILING ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(MINGW OR CYGWIN OR WIN32)
	set(UTIL_SEARCH_CMD where)
elseif(UNIX OR APPLE)
	set(UTIL_SEARCH_CMD which)
endif()

set(TOOLCHAIN_PREFIX arm-none-eabi-)

execute_process(
	COMMAND ${UTIL_SEARCH_CMD} ${TOOLCHAIN_PREFIX}gcc
	OUTPUT_VARIABLE BINUTILS_PATH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

#get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)


set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size tool")

set(CMAKE_SYSROOT ${ARM_TOOLCHAIN_DIR}/../arm-none-eabi)
set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/sdk/include)
set($ENV{BOLOS_SDK} ${CMAKE_CURRENT_SOURCE_DIR}/libs/sdk)

file(GLOB SOURCES src/*)
file(GLOB SOURCES_SECP libs/secp256k1/src/*)
file(GLOB HEADERS_SECP libs/secp256k1/include/*)

add_definitions(-DOS_IO_SEPROXYHAL -DIO_SEPROXYHAL_BUFFER_SIZE_B=128)
add_definitions(-DHAVE_BAGL -DHAVE_SPRINTF)
add_definitions(-DHAVE_IO_USB -DHAVE_L4_USBLIB -DIO_USB_MAX_ENDPOINTS=7 -DIO_HID_EP_LENGTH=64 -DHAVE_USB_APDU)
add_definitions(-DAPPVERSION="1.0.0")

include(modules/ledger_common.cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/secp256k1/include)

add_custom_target(ledger-build
                  COMMAND make clean && make
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  )

add_executable(ledger-minter-dummy ${SOURCES} ${SOURCES_SECP} ${HEADERS_SECP})
target_link_libraries(ledger-minter-dummy m gcc c)



