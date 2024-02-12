#pragma once

#include <concepts>


namespace EasyRobotCommands {
namespace InnerConcepts {

template<typename T, typename caller>
concept can_triggered_from_caller = requires(T a, caller& c) {
    { a.triggered_from(c) } -> std::same_as<void>;
};

template<typename T, typename caller>
concept can_triggered_from_caller_sharedptr = requires(T a, typename caller::SharedPtr c) {
    { a.triggered_from(c) } -> std::same_as<void>;
};

template<typename T>
concept has_triggered_from_member = requires {
    typename T::triggered_from;
};

}

template<typename T>
concept can_trigger = requires(T& a) {
    { a.trigger()} -> std::same_as<void>;
    // typename  decltype(a.register_trigger_operation);
};

template<typename T, typename caller>
concept can_triggered_from = 
    InnerConcepts::can_triggered_from_caller<T, caller> || 
    InnerConcepts::can_triggered_from_caller_sharedptr<T, caller>;

template<typename self, typename caller>
concept is_trigger_tree_node_concept = can_trigger<self> && can_triggered_from<self, caller>;

template<typename msg>
concept is_msg_has_SharedPtr_type = requires { typename msg::SharedPtr; };

template<typename T, typename msg>
concept is_struct_data_of_msg = requires(T self, typename msg::SharedPtr m) {
    typename T::topic_name;
    typename T::MsgTypeName;
    {self = m};
};

template<typename T>
concept has_get_structure_data_method = requires(T& a) {
    {a.get_structure_data()};
};

template<typename T>
concept has_get_struct_data_method = requires(T& a) {
    {a.get_struct_data()};
};
}