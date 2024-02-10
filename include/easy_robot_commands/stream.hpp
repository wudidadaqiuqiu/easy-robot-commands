#pragma once
#include <cstdint>
#include <mutex>
#include <vector>

#include "container/multithread_stream.hpp"
#include "protocol/serialized_protocol.hpp"
#include "easy_robot_commands/shared_member/base_caller_template.hpp"
#include "easy_robot_commands/shared_member/concepts.hpp"
#include "easy_robot_commands/shared_member/trigger_operation.hpp"

namespace EasyRobotCommands {

template <stream_size_t stream_size, ISProtocolConfigable protocol_confiT>
class Stream {
   public:
    Stream()
        : inner_stream(), opera_when_triggered([](Stream&) { /*do nothing*/ }){};
    void trigger() { opera_when_triggered.trigger(*this); };

    template <Packable caller_t>
    void triggered_from(caller_t& caller) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            PackGenerator<caller_t, protocol_confiT> it(caller);
            inner_stream << it;
        }
        trigger();
    }
    void register_trigger_operation(const trigger_operation<Stream>& operation) {
        opera_when_triggered.change_operation(operation.get_operation());
    };

    template <ISBlockedConsumer T>
    Stream& operator>>(T& consumer) {
        using state_e = multithread_stream<stream_size>::stream_state_e;
        while(inner_stream.get_state() != state_e::stream_empty)
            inner_stream >> consumer;
        return *this;
    }

   private:
    mutable std::mutex mutex;
    multithread_stream<stream_size> inner_stream;
    trigger_operation<Stream> opera_when_triggered;
};

}  // namespace EasyRobotCommands