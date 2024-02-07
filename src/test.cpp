#include <iostream>
#include "easy_robot_commands/robot_modules_mode.hpp"

int main() {
    using easy_robot_commands::msg::RobotModulesMode;
    // EasyRobotCommands::robot_modules_mode module_mode{};
    // std::cout << EasyRobotCommands::robot_modules_mode::my_class_name;
    EasyRobotCommands::ea_base_caller<RobotModulesMode> m{};
    std::cout << EasyRobotCommands::StructDataT<RobotModulesMode>::topic_name << std::endl;
}