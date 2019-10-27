cmake_minimum_required(VERSION 3.5)
project(controller)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)
set(Boost_DEBUG 1)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
include_directories(/usr/include/eigen3)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

find_package(Boost 1.65.1 COMPONENTS thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
add_executable(timer_member_function src/member_function.cpp src/protocol.cpp src/frame.cpp src/serial.cpp)
ament_target_dependencies(timer_member_function rclcpp boost)
target_link_libraries(timer_member_function ${Boost_LIBRARIES})

install(TARGETS
  timer_member_function
  DESTINATION lib/${PROJECT_NAME}
)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
