cmake_minimum_required(VERSION 3.10)
project(kernel-de)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    src/main.cpp
    src/filesystem_funcs.cpp
    src/collector.cpp
    src/collecting_funcs.cpp
    src/metric_calc_funcs.cpp
    src/metric_form_funcs.cpp
)

add_executable(kernel-de ${SOURCES})

target_include_directories(kernel-de PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_options(kernel-de PRIVATE -static)

configure_file(
  ${PROJECT_SOURCE_DIR}/configs/kernel-de.conf
  ${PROJECT_BINARY_DIR}/kernel-de.conf
  COPYONLY
)
