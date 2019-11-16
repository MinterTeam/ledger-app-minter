cmake_minimum_required(VERSION 3.10)
project(ledger-app-minter
        VERSION 0.1.0
        DESCRIPTION "Minter Ledger CMake wip project"
        HOMEPAGE_URL "https://github.com/MinterTeam/ledger-app-minter"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
	execute_process(COMMAND cmd /C "echo ${CMAKE_PROJECT_VERSION} > ${CMAKE_CURRENT_SOURCE_DIR}/version")
else ()
	execute_process(COMMAND bash -c "echo ${CMAKE_PROJECT_VERSION} > ${CMAKE_CURRENT_SOURCE_DIR}/version")
endif ()

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
#target_link_libraries(ledger-minter-dummy m gcc c)



