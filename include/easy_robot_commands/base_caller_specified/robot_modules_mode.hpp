#pragma once

#include "easy_robot_commands/msg/robot_modules_mode.hpp"
#include "easy_robot_commands/shared_member/base_caller_template.hpp"
#include "easy_robot_commands/shared_member/pkg_info.hpp"
#include "easy_robot_commands/shared_member/trigger_operation.hpp"

namespace eamsg = easy_robot_commands::msg;

template <>
struct EasyRobotCommands::StructDataT<eamsg::RobotModulesMode> {
#pragma pack(1)
    struct data_t {
        uint8_t master_enable : 1;
        uint8_t yaw_enable : 1;
        uint8_t pitch_enable : 1;
        uint8_t chassis_enable : 1;
    };
    data_t data;
    static_assert((sizeof(data_t) == 1));
#pragma pack()
    /* assign operate*/
    StructDataT& operator=(const eamsg::RobotModulesMode::SharedPtr msgptr) {
        this->data.master_enable = msgptr->master_enable;
        this->data.chassis_enable = msgptr->chassis_enable;
        this->data.pitch_enable = msgptr->pitch_enable;
        this->data.yaw_enable = msgptr->yaw_enable;
        return *this;
    }

    static constexpr std::string_view MsgTypeName = "RobotModulesMode";
    static constexpr std::string_view topic_name = "robot_modules_mode";
    static constexpr uint8_t ID = 0x11;
};