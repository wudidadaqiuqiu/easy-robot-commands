#ifndef REGISTER_COMMANDS_HPP
#define REGISTER_COMMANDS_HPP

#include "easy_robot_commands/msg/robot_modules_mode.hpp"
#include "easy_robot_commands/msg_serialized_funcs.hpp"
#include "rclcpp/rclcpp.hpp"

namespace EasyRobotCommands {

// template <typename T>
// struct TypeNameHelper;

// template <>
// struct TypeNameHelper<erm_msg::RobotModulesMode> {
//     static std::string name() { return std::string("robot_modules_mode"); }
// };

// 函数参数包括回调函数和 Node 对象，返回一个 Subscription 对象
// template <typename T>
// typename rclcpp::Subscription<T>::SharedPtr create_subscription_with_callback(
//     rclcpp::Node::SharedPtr node, MutexStream& stream) {
//     (void)stream;
//     (void)node;
// }

// template <>
// rclcpp::Subscription<erm_msg::RobotModulesMode>::SharedPtr create_subscription_with_callback<erm_msg::RobotModulesMode>(
//     rclcpp::Node::SharedPtr node, MutexStream& stream) {
//     // 为了让callback能够操作node
//     auto lambda = [node, &stream](const erm_msg::RobotModulesMode::SharedPtr msg) {
//         (void) msg;
//         std::vector<uint8_t> b = {1,1,1,1};
//         stream.put(b);
//     };
//     auto subtopic_name(TypeNameHelper<erm_msg::RobotModulesMode>::name());
//     return node->create_subscription<erm_msg::RobotModulesMode>("easy_robot_commands/"+subtopic_name, 10, lambda);
// }

}  // namespace EasyRobotCommands


#endif