#include <iostream>
#include "easy_robot_commands/robot_modules_mode.hpp"

int main() {
    EasyRobotCommands::robot_modules_mode module_mode{};
    std::cout << EasyRobotCommands::robot_modules_mode::my_class_name;
}