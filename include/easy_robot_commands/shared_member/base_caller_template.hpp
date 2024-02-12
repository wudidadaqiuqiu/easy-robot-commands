#pragma once

#include <mutex>

#include "easy_robot_commands/shared_member/trigger_operation.hpp"
#include "easy_robot_commands/shared_member/concepts.hpp"
namespace EasyRobotCommands {

template <typename msg_t> requires is_msg_has_SharedPtr_type<msg_t>
struct StructDataT {
    StructDataT() requires is_struct_data_of_msg<StructDataT, msg_t> {};
    struct data_t {
        /* data field */
    };
    data_t data;
    // ...
    /* assign operate*/
    StructDataT& operator=(const typename msg_t::SharedPtr msg) {
        return (*this);
    }
    static constexpr std::string_view MsgTypeName = "MsgTypeName";
    static constexpr std::string_view topic_name = "topic_name";
    static constexpr uint8_t ID = 0x00;
};


template <typename msg_t>
class ea_base_caller {
   public:
   using struct_data_t = StructDataT<msg_t>;
    ea_base_caller() requires is_trigger_tree_node_concept<ea_base_caller, msg_t> 
        : struct_data{}, opera_when_triggered([](ea_base_caller&) { /*do nothing*/ }){};
    void triggered_from(typename msg_t::SharedPtr msgptr);
    void trigger() { opera_when_triggered.trigger(*this); };
    void register_trigger_operation(const trigger_operation<ea_base_caller>& operation) {
        opera_when_triggered.change_operation(operation.get_operation());
    };
    const StructDataT<msg_t>& get_struct_data() const {
        std::lock_guard<std::mutex> lock(struct_data_mutex);
        return struct_data;
    };

    const StructDataT<msg_t>::data_t& get_structure_data() const {
        std::lock_guard<std::mutex> lock(struct_data_mutex);
        return struct_data.data;
    }
   private:
    mutable std::mutex struct_data_mutex;
    StructDataT<msg_t> struct_data;
    trigger_operation<ea_base_caller> opera_when_triggered;
};

template <typename msg_t>
inline void ea_base_caller<msg_t>::triggered_from(typename msg_t::SharedPtr msgptr) {
    {
        std::lock_guard<std::mutex> lock(struct_data_mutex);
        this->struct_data = msgptr;
    }
    trigger();
}

template <typename T, can_trigger U>
T& trigger_chain_join(T& l, U& r) {
    auto lambda = [&l](U& trigger) {
        l.triggered_from(trigger);
    };
    r.register_trigger_operation(trigger_operation<U>(lambda));
    return l;
}

template <typename msg_t>
struct BaseCallerWrap {
    using caller = ea_base_caller<msg_t>;
};


}  // namespace EasyRobotCommands