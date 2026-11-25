cmake_minimum_required(VERSION 3.21)
project(eram_gui VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)

add_library(eram_core STATIC
    src/core/Simulation.cpp
    src/core/Simulation.h
)
target_include_directories(eram_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_features(eram_core PUBLIC cxx_std_20)

add_executable(eram_gui
    src/main.cpp
    src/gui/MainWindow.cpp
    src/gui/MainWindow.h
    src/gui/SectorView.cpp
    src/gui/SectorView.h
)
target_link_libraries(eram_gui PRIVATE Qt6::Widgets eram_core)
set_target_properties(eram_gui PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)

include(CTest)
if(BUILD_TESTING)
    add_executable(eram_core_tests tests/SimulationTests.cpp)
    target_link_libraries(eram_core_tests PRIVATE eram_core)
    add_test(NAME eram_core_tests COMMAND eram_core_tests)
endif()
