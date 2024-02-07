#pragma once


#include <mutex>

#include "easy_robot_commands/shared_member/pkg_info.hpp"
#include "easy_robot_commands/shared_member/trigger_operation.hpp"

namespace EasyRobotCommands {

template <typename msg_t>
struct StructDataT {
    /* data field */
    // ...
    /* assign operate*/
    StructDataT& operator=(const typename msg_t::SharedPtr msg) {
        return (*this);
    }
    static const std::string MsgTypeName;
    static const std::string topic_name;
};


template <typename msg_t>
class ea_base_caller {
   public:
    ea_base_caller() : struct_data(), opera_when_triggered([](ea_base_caller&) { /*do nothing*/ }){};
    void triggered_from_(typename msg_t::SharedPtr msgptr);
    void register_trigger_operation(const trigger_operation<ea_base_caller>& operation) {
        opera_when_triggered.change_operation(operation.get_operation());
    };
    const StructDataT<msg_t>& get_struct_data() const {
        std::lock_guard<std::mutex> lock(struct_data_mutex);
        return struct_data;
    };

   private:
    mutable std::mutex struct_data_mutex;
    StructDataT<msg_t> struct_data;
    trigger_operation<ea_base_caller> opera_when_triggered;
};

template <typename msg_t>
inline void ea_base_caller<msg_t>::triggered_from_(typename msg_t::SharedPtr msgptr) {
    {
        std::lock_guard<std::mutex> lock(struct_data_mutex);
        this->struct_data = msgptr;
    }
    opera_when_triggered.triggered(*this);
}

}  // namespace EasyRobotCommands