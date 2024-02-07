#pragma once

#include <concepts>


namespace EasyRobotCommands {
namespace InnerConcepts {

template<typename T, typename caller>
concept can_triggered_from_caller = requires(T a, caller c) {
    { a.triggered_from(c) } -> std::same_as<void>;
};

template<typename T, typename caller>
concept can_triggered_from_caller_sharedptr = requires(T a, typename caller::SharedPtr c) {
    { a.triggered_from(c) } -> std::same_as<void>;
};

}

template<typename T>
concept can_trigger = requires(T a) {
    { a.trigger()} -> std::same_as<void>;
};

template<typename T, typename caller>
concept can_triggered_from = 
    InnerConcepts::can_triggered_from_caller<T, caller> || 
    InnerConcepts::can_triggered_from_caller_sharedptr<T, caller>;

template<typename T, typename caller>
concept is_trigger_chain_node_concept = can_trigger<T> && can_triggered_from<T, caller>;

template<typename msg>
concept is_msg_has_SharedPtr_type = requires { typename msg::SharedPtr; };

template<typename T, typename msg>
concept is_struct_data_of_msg = requires(T self, typename msg::SharedPtr m) {
    typename T::topic_name;
    typename T::MsgTypeName;
    {self = m};
};
}