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

    template <typename caller_t>
        requires is_trigger_tree_node_concept<Stream, caller_t> &&
                 has_get_structure_data_method<caller_t> &&
                 has_get_struct_data_method<caller_t>
    void triggered_from(caller_t& caller) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            // process triggered data
            // caller.get_struct_data()
            
        }
        trigger();
    }
    void register_trigger_operation(const trigger_operation<Stream>& operation) {
        opera_when_triggered.change_operation(operation.get_operation());
    };

   private:
    mutable std::mutex mutex;
    multithread_stream<stream_size> inner_stream;
    trigger_operation<Stream> opera_when_triggered;
};

}  // namespace EasyRobotCommands