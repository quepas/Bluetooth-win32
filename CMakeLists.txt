cmake_minimum_required(VERSION 3.0)

project(bluetooth-win32 VERSION "0.1.0" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/)

add_subdirectory(enumerate_bt_devices)
add_subdirectory(enumerate_bt_radio_info)
add_subdirectory(make_bt_radio_discoverable)
add_subdirectory(select_bt_device_dialog)
