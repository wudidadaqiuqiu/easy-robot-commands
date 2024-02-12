#pragma once

#include "easy_robot_commands/msg/robot_chassis_velocity.hpp"
#include "easy_robot_commands/shared_member/base_caller_template.hpp"
#include "easy_robot_commands/shared_member/trigger_operation.hpp"

namespace EasyRobotCommands {
namespace eamsg = easy_robot_commands::msg;
}

template <>
struct EasyRobotCommands::StructDataT<EasyRobotCommands::eamsg::RobotChassisVelocity> {
#pragma pack(1)
    struct data_t {
        float vx;
        float vy;
        float wz;
    };
    data_t data;
    static_assert((sizeof(data_t) == 12));
#pragma pack()
    /* assign operate*/
    StructDataT& operator=(const eamsg::RobotChassisVelocity::SharedPtr msgptr) {
        data.vx = msgptr->vx;
        data.vy = msgptr->vy;
        data.wz = msgptr->wz;
        return *this;
    }

    static constexpr std::string_view MsgTypeName = "RobotChassisVelocity";
    static constexpr std::string_view topic_name = "robot_chassis_velocity";
    static constexpr uint8_t ID = 0x12;
};