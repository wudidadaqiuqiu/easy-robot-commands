#ifndef EASY_ROBOT_COMMANDS__ROBOT_MODULES_MODE_HPP
#define EASY_ROBOT_COMMANDS__ROBOT_MODULES_MODE_HPP

#include <mutex>

#include "easy_robot_commands/msg/robot_modules_mode.hpp"
#include "easy_robot_commands/shared_member/pkg_info.hpp"
#include "easy_robot_commands/shared_member/trigger_operation.hpp"

#define EACLASS_NAME robot_modules_mode
#define EAMSG_NAME RobotModulesMode
#define EA_STR(a) #a
#define EASTR(a) EA_STR(a)

namespace EasyRobotCommands {

namespace msg = easy_robot_commands::msg;
class robot_modules_mode {
   public:
    struct struct_data_t {
        uint8_t master_enable : 1;
        uint8_t yaw_enable : 1;
        uint8_t pitch_enable : 1;
        uint8_t chassis_enable : 1;
    };
    static const std::string my_class_name;
    static const std::string my_corresponding_msg_name;

    robot_modules_mode() : struct_data(), opera_when_triggered([](robot_modules_mode&) { /*do nothing*/ }){};
    void triggered_from_(msg::RobotModulesMode::SharedPtr msgptr);
    void register_trigger_operation(const trigger_operation<robot_modules_mode>& operation) {
        opera_when_triggered.change_operation(operation.get_operation());
    };
    const struct_data_t& get_struct_data() const {
        std::lock_guard<std::mutex> lock(struct_data_mutex);
        return struct_data;
    };

   private:
    mutable std::mutex struct_data_mutex;
    struct_data_t struct_data;
    trigger_operation<robot_modules_mode> opera_when_triggered;

    EACLASS_NAME& operator=(const msg::EAMSG_NAME::SharedPtr msgptr) {
        std::lock_guard<std::mutex> lock(struct_data_mutex);
        struct_data.master_enable = msgptr->master_enable;
        struct_data.chassis_enable = msgptr->chassis_enable;
        struct_data.pitch_enable = msgptr->picth_enable;
        struct_data.yaw_enable = msgptr->yaw_enable;
        return *this;
    };
};

const std::string robot_modules_mode::my_class_name = (EASTR(EACLASS_NAME));
const std::string robot_modules_mode::my_corresponding_msg_name = (EASTR(EAMSG_NAME));

inline void robot_modules_mode::triggered_from_(msg::RobotModulesMode::SharedPtr msgptr) {
    *this = msgptr;
    opera_when_triggered.triggered(*this);
}

}  // namespace EasyRobotCommands

#undef EACLASS_NAME
#undef EAMSG_NAME
#undef EA_STR
#undef EASTR
#endif
