cmake_minimum_required(VERSION 3.5)
project(PistolsAtDawn)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_DEBUG_POSTFIX "-dbg")

find_package(Eigen3 REQUIRED NO_MODULE)
# find_package for eigen does not work sometimes and does not add library
if (NOT TARGET Eigen3::Eigen)
  include_directories(${EIGEN3_INCLUDE_DIR})
endif()

add_library(PistolsAtDawn
  src/Dual.cpp
  src/Loss.cpp
  src/Optimization.cpp)

target_include_directories(PistolsAtDawn PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:src>)

if (TARGET Eigen3::Eigen)
  # we can relay on it to provide transitive dependency
  target_link_libraries(PistolsAtDawn PUBLIC Eigen3::Eigen)
endif()

add_library(PistolsAtDawn::PistolsAtDawn ALIAS PistolsAtDawn)

#### Installation of library
install(DIRECTORY src/ DESTINATION include
        FILES_MATCHING PATTERN "*.h")

install(TARGETS PistolsAtDawn
        ARCHIVE        DESTINATION lib/static
        LIBRARY        DESTINATION lib
        RUNTIME        DESTINATION bin
        PUBLIC_HEADER  DESTINATION include)

### Other (tests, examples, etc)

option(BUILD_EXAMPLE "Build example" ON)

if (BUILD_EXAMPLE)
  add_executable(sphere_example
          example/main.cpp
          example/Sphere.cpp
          example/SyntheticData.cpp
          )
  target_link_libraries(sphere_example PistolsAtDawn::PistolsAtDawn)
endif()
