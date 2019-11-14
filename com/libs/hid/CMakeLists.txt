project(hidapi)
cmake_minimum_required(VERSION 3.10)


set(HEADERS
    hidapi/hidapi.h
    )


set(SOURCES
    ${HEADERS}
    )

if (APPLE)
	message(STATUS "hidapi: MacOS")
	set(SOURCES ${SOURCES} mac/hid.c)
elseif (MSVC)
	message(STATUS "hidapi: Windows")
	set(SOURCES ${SOURCES} windows/hid.c)
elseif (MINGW)
	message(STATUS "hidapi: MinGW")
	#TODO
elseif (CYGWIN)
	message(STATUS "hidapi: Windows Cygwin")
	set(SOURCES ${SOURCES} libusb/hid.c)
	find_library(USB_LIB usb REQUIRED)
elseif (ANDROID_PLATFORM)
	message(STATUS "hidapi: Android")
	#TODO
else ()
	message(STATUS "hidapi: Linux")
	set(SOURCES ${SOURCES} linux/hid.c)
endif ()

add_library(hidapi STATIC ${SOURCES})
target_include_directories(hidapi PUBLIC hidapi)

if (CYGWIN)
	find_library(USB_LIB usb)
	if (USB_LIB-NOT_FOUND)
		message(FATAL_ERROR "libusb not found!")
	endif ()
	message(STATUS ${USB_LIB})
	target_link_libraries(hidapi ${USB_LIB})
elseif (UNIX AND NOT APPLE)
	find_library(UDEV_LIB udev REQUIRED)
	if (UDEV_LIB-NOT_FOUND)
		message(FATAL_ERROR "libudev not found!")
	endif ()

	find_library(USB_LIB usb REQUIRED)
	if (USB_LIB-NOT_FOUND)
		message(FATAL_ERROR "libusb not found!")
	endif ()

	message(STATUS "libudev: ${UDEV_LIB}")
	message(STATUS "libusb: ${USB_LIB}")

	target_link_libraries(${PROJECT_NAME} ${UDEV_LIB})
	target_link_libraries(${PROJECT_NAME} ${USB_LIB})
elseif (APPLE)
	find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
	find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
	find_library(USB_LIB usb REQUIRED)
	message(STATUS "libusb: ${USB_LIB}")
	message(STATUS "Framework IOKit: ${IOKIT_FRAMEWORK}")
	message(STATUS "Framework CoreFoundation: ${CORE_FOUNDATION_FRAMEWORK}")
	target_link_libraries(${PROJECT_NAME} ${USB_LIB})
	target_link_libraries(${PROJECT_NAME} ${IOKIT_FRAMEWORK})
	target_link_libraries(${PROJECT_NAME} ${CORE_FOUNDATION_FRAMEWORK})

endif ()