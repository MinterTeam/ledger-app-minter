project(minter-ledger-com
        LANGUAGES CXX C)
cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
include(ConanInit)
conan_init()

add_subdirectory(libs/hid)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_definitions(-DMINTER_HID_LOG=1)
endif ()

set(SOURCES
    src/hid_dev.cpp
    src/hid_dev.h
    include/hid_config.h
    src/hid_dev_info.cpp
    src/hid_dev_info.h
    src/hid.cpp
    src/hid.h
    src/hid_frame.cpp
    src/hid_frame.h
    src/hid_frame_apdu.h
    src/hid_frame_apdu.cpp
    )

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})

find_library(USB_LIB usb REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/hid)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} hidapi)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::minter_tx)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::fmt)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::boost_program_options)

set(TESTS ON)
if (TESTS)
	set(gtest_force_shared_crt ${ENABLE_SHARED} CACHE BOOL "" FORCE)
	add_definitions(-DTEST_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/tests")
	set(PROJECT_NAME_TEST ${PROJECT_NAME}-test)

	if (APPLE)
		add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
		add_definitions(-D__GLIBCXX__)
	endif ()

	if (MSVC)
		string(REGEX REPLACE "\\/W4" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	endif ()

	set(TEST_SOURCES
	    tests/main.cpp
	    ${SOURCES}
	    )

	add_executable(${PROJECT_NAME_TEST} ${TEST_SOURCES})
	if (NOT MSVC)
		target_compile_options(${PROJECT_NAME_TEST} PUBLIC -Wno-unused-parameter)
	endif ()
	target_link_libraries(${PROJECT_NAME_TEST} PRIVATE CONAN_PKG::gtest)
	target_link_libraries(${PROJECT_NAME_TEST} PUBLIC hidapi)
	target_include_directories(${PROJECT_NAME_TEST} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif ()
