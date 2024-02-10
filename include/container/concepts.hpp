#pragma once
#include <cstdint>
#include <ranges>

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

template <typename T>
concept ISBlockedConsumer = requires(T& a, const uint8_t* data, size_t len) {
    { a.blocked_consume(data, len) };
};

}  // namespace EasyRobotCommands
