cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_TOOLCHAIN_FILE /usr/local/etc/arduino-cmake/ArduinoToolchain.cmake)
set(CMAKE_CXX_STANDARD 14)
set(PROJECT_NAME ArduinoNanoLowPowerSleepMode)

set(${PROJECT_NAME}_BOARD nano)
set(ARDUINO_CPU atmega328old)
project(${PROJECT_NAME})

# Define additional source and header files or default arduino sketch files
# These files are now added to the sketch file, if the sketch is defined.
# set(${PROJECT_NAME}_SRCS)
# set(${PROJECT_NAME}_HDRS)

### Additional static libraries to include in the target.
# set(${PROJECT_NAME}_LIBS)

### Main sketch file
set(${PROJECT_NAME}_SKETCH arduino_nano_low_power_sleep_mode.ino)

### Add project directories into the build
# add_subdirectory()

### Additional settings to add non-standard or your own Arduino libraries.
# For this example (libs will contain additional arduino libraries)
# An Arduino library my_lib will contain files in libs/my_lib/: my_lib.h, my_lib.cpp + any other cpp files
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libs ${CMAKE_CURRENT_SOURCE_DIR}/~/Documents/Arduino/libraries)

# This is no longer need but left in for those occasions where automatics do not work out. 
# Current Arduino.cmake will recurse into any libraries which contain source files in src/ subdirectory. 
# Files in subdirectories whose name starts with cmake-build or examples are excluded
# For nested library sources replace ${LIB_NAME} with library name for each library
# set(_RECURSE true)

#### Additional settings for programmer. From programmers.txt
set(${PROJECT_NAME}_PROGRAMMER avrispmkii)
set(${PROJECT_NAME}_PORT /dev/cu.usbserial-A5069RR4)
# set(nano.upload.speed 9600)

## Verbose build process
set(${PROJECT_NAME}_AFLAGS -v)

generate_arduino_firmware(${PROJECT_NAME})


print_board_list()
print_programmer_list()
