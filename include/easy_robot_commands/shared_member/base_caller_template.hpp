#pragma once


#include <mutex>

#include "easy_robot_commands/shared_member/pkg_info.hpp"
#include "easy_robot_commands/shared_member/trigger_operation.hpp"

namespace EasyRobotCommands {

template <typename msg_t>
class ea_base_caller {
   public:
    struct struct_data_t {

    };
    static const std::string my_class_name;
    static const std::string my_corresponding_msg_name;

    ea_base_caller() : struct_data(), opera_when_triggered([](ea_base_caller&) { /*do nothing*/ }){};
    void triggered_from_(msg_t::SharedPtr msgptr);
    void register_trigger_operation(const trigger_operation<ea_base_caller>& operation) {
        opera_when_triggered.change_operation(operation.get_operation());
    };
    const struct_data_t& get_struct_data() const {
        std::lock_guard<std::mutex> lock(struct_data_mutex);
        return struct_data;
    };

   private:
    mutable std::mutex struct_data_mutex;
    struct_data_t struct_data;
    trigger_operation<ea_base_caller> opera_when_triggered;

    ea_base_caller& operator=(const msg_t::SharedPtr msgptr) {
        std::lock_guard<std::mutex> lock(struct_data_mutex);
        return *this;
    };
};

template <typename msg_t>
const std::string ea_base_caller<msg_t>::my_class_name;
template <typename msg_t>
const std::string ea_base_caller<msg_t>::my_corresponding_msg_name;

template <typename msg_t>
inline void ea_base_caller<msg_t>::triggered_from_(msg_t::SharedPtr msgptr) {
    *this = msgptr;
    opera_when_triggered.triggered(*this);
}

}  // namespace EasyRobotCommands