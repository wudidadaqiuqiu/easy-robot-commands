cmake_minimum_required(VERSION 3.8)
project(easy_robot_commands)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rclcpp REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/RobotModulesMode.msg"
)
# export 依赖项 依赖于该包无需显式声明依赖
ament_export_dependencies(rosidl_default_runtime)


# new library (后面的参数是要被调用方使用的)
add_library(${PROJECT_NAME}_funcs SHARED
  src/register_commands.cpp
  include/container/multithread_stream.hpp
  include/${PROJECT_NAME}/robot_modules_mode.hpp
  include/${PROJECT_NAME}/register_commands.hpp
  include/${PROJECT_NAME}/msg_serialized_funcs.hpp
  include/${PROJECT_NAME}/stream.hpp
  include/${PROJECT_NAME}/shared_member/trigger_operation.hpp
  include/${PROJECT_NAME}/shared_member/base_caller_templete.hpp
)

# 添加头文件地址
target_include_directories(${PROJECT_NAME}_funcs PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

#依赖
ament_target_dependencies(${PROJECT_NAME}_funcs 
  rclcpp
)
#依赖自身定义msg
rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME} rosidl_typesupport_cpp)
target_link_libraries(${PROJECT_NAME}_funcs "${cpp_typesupport_target}")

add_executable(eatest src/test.cpp)
target_link_libraries(eatest "${cpp_typesupport_target}")
target_include_directories(eatest PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# 安装头文件
install(DIRECTORY include/
  DESTINATION include
)

# install 库 in pkg
# depend 时只需 ament_target_dependencies(a, depend_pkg_name)
install(
  TARGETS 
  ${PROJECT_NAME}_funcs
  EXPORT ${PROJECT_NAME}_funcs
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install (TARGETS eatest)

## export 让调用方能找到
# export library
ament_export_targets(${PROJECT_NAME}_funcs HAS_LIBRARY_TARGET)
# export dependencies
ament_export_dependencies(
  rclcpp
)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()



ament_package()
