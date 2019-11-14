project(ledger-app-minter
        LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/sdk/include)
set($ENV{BOLOS_SDK} ${CMAKE_CURRENT_SOURCE_DIR}/libs/sdk)

file(GLOB SOURCES src/*)

add_definitions(-DOS_IO_SEPROXYHAL -DIO_SEPROXYHAL_BUFFER_SIZE_B=128)
add_definitions(-DHAVE_BAGL -DHAVE_SPRINTF)
add_definitions(-DHAVE_IO_USB -DHAVE_L4_USBLIB -DIO_USB_MAX_ENDPOINTS=7 -DIO_HID_EP_LENGTH=64 -DHAVE_USB_APDU)
add_definitions(-DAPPVERSION="1.0.0")

add_custom_target(ledger-build
                  COMMAND make clean && make
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  )

add_executable(ledger-minter-dummy ${SOURCES})
target_link_libraries(ledger-minter-dummy m gcc c)

add_subdirectory(com)



