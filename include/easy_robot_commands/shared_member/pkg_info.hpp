#pragma once

#include <tuple>

#include "easy_robot_commands/msg/robot_chassis_velocity.hpp"
#include "easy_robot_commands/msg/robot_modules_mode.hpp"
#include "easy_robot_commands/shared_member/base_caller_template.hpp"
#include "easy_robot_commands/base_caller_specified/robot_modules_mode.hpp"
#include "easy_robot_commands/base_caller_specified/robot_chassis_velocity.hpp"

namespace EasyRobotCommands {
namespace InnerSim {
namespace em = easy_robot_commands::msg;
}

template <typename... Args>
struct MSGPack {
    using MSGTuple = std::tuple<Args...>;
    using CallerTuple = std::tuple<ea_base_caller<Args>...>;
    static constexpr std::string_view pkg_name = "easy_robot_commands";
};

using AllMSGPackT = MSGPack<InnerSim::em::RobotModulesMode, InnerSim::em::RobotChassisVelocity>;

auto make_base_callers_tuple() {
    return AllMSGPackT::CallerTuple();
}

}  // namespace EasyRobotCommands