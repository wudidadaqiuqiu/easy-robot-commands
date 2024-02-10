#pragma once

namespace EasyRobotCommands {

template <typename T>
concept Iterable = requires(T& a) {
    { ++a };
    { *a } ;
    { a.is_ended() };
};

template <typename T>
concept ISProducer = requires(T& a) {
    Iterable<T>;
    { a.minmax_len() };
};
}  // namespace EasyRobotCommands
