#pragma once

#include <mutex>

#include "easy_robot_commands/msg/robot_modules_mode.hpp"
#include "easy_robot_commands/shared_member/pkg_info.hpp"
#include "easy_robot_commands/shared_member/trigger_operation.hpp"
#include "easy_robot_commands/shared_member/base_caller_template.hpp"

namespace eamsg = easy_robot_commands::msg;

template <>
struct EasyRobotCommands::StructDataT<eamsg::RobotModulesMode> {
    uint8_t master_enable : 1;
    uint8_t yaw_enable : 1;
    uint8_t pitch_enable : 1;
    uint8_t chassis_enable : 1;

    /* assign operate*/
    StructDataT& operator=(const eamsg::RobotModulesMode::SharedPtr msgptr) {
        this->master_enable = msgptr->master_enable;
        this->chassis_enable = msgptr->chassis_enable;
        this->pitch_enable = msgptr->picth_enable;
        this->yaw_enable = msgptr->yaw_enable;
        return *this;
    };

    static const std::string MsgTypeName;
    static const std::string topic_name;
};