#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>

#include "container/concepts.hpp"

namespace EasyRobotCommands {

#define STREAM_DEBUG 0

template <typename T1>
inline void debug_log(T1 mes, bool endlit = true) {
    if (STREAM_DEBUG) {
        std::cout << mes;
        if (endlit) std::cout << std::endl;
    }
}

static constexpr bool strema_debug_iterator = true;
 
template<typename T>
inline void DEBUG_STREAM(bool con, T mes, bool endlit = true) {
    if (!con) return;
    std::cout << mes;
    if (endlit) std::cout << std::endl;
}
inline void debug_endl() {
    if (STREAM_DEBUG) {
        std::cout << std::endl;
    }
}
using stream_size_t = size_t;

template <stream_size_t stream_size>
// multi producer with only one consumer in fifo mode
class multithread_stream {
   public:
    enum stream_state_e {
        stream_empty,
        stream_normal,
        stream_full,
    };
    multithread_stream() : state(stream_empty),
                           start(buffer),
                           endp1(buffer),
                           protected_start(buffer),
                           protected_endp1(buffer),
                           start_buf(buffer),
                           end_buf(buffer + stream_size){};
    stream_state_e get_state() const { return state; };
    bool is_overrun(stream_size_t len = 0, bool locked = true) const {
        if (len >= stream_size) return true;
        if (locked) {
            std::lock_guard<std::mutex> lock(mutex);
            stream_size_t wait_len(
                (protected_endp1 < protected_start) ? (stream_size - (protected_start - protected_endp1))
                                                    : (protected_endp1 - protected_start));
            uint8_t* temp = circular_ptr_add(len);
            stream_size_t after_add_wait_len((temp < protected_start) ? (stream_size - (protected_start - temp))
                                                                      : (temp - protected_start));
            return (wait_len >= after_add_wait_len);
        } else {
            stream_size_t wait_len(
                (protected_endp1 < protected_start) ? (stream_size - (protected_start - protected_endp1))
                                                    : (protected_endp1 - protected_start));
            debug_log<std::string>("wait len is ");
            debug_log<stream_size_t>(wait_len);
            debug_endl();
            uint8_t* temp = circular_ptr_add(len);
            stream_size_t after_add_wait_len((temp < protected_start) ? (stream_size - (protected_start - temp))
                                                                      : (temp - protected_start));
            debug_log<std::string>("after_add_wait_len is ");
            debug_log<stream_size_t>(after_add_wait_len);
            debug_endl();
            return (wait_len >= after_add_wait_len);
        }
    };
    bool is_stream_empty_after_next_consume_complete() {
        return (endp1 >= start) || (endp1 == buffer);
    };
    stream_state_e consumer_update_protected_range() {
        std::lock_guard<std::mutex> lock(mutex);
        protected_start = start;
        if (protected_start == protected_endp1) return (state = stream_empty);
        return (state = stream_normal);
    };
    stream_state_e add_with_caution(const uint8_t* data, stream_size_t len) {
        std::lock_guard<std::mutex> plock(producer_mutex);
        std::lock_guard<std::mutex> lock(mutex);
        if (len == 0) return (state = (state == stream_empty) ? stream_empty : stream_normal);
        if (is_overrun(len, false)) return (state = stream_full);
        circular_ptr_copy(data, len);
        protected_endp1 = endp1 = circular_ptr_add(len);
        return (state = stream_normal);
    };
    // consume 可以是阻塞的，如果是阻塞的需要传参说明，非阻塞需要在完成后调用consumer_update_protected_range
    stream_state_e consume_with_best_effort(
        const std::function<void(uint8_t* start, stream_size_t len)>& consume, bool blocked) {
        stream_size_t consume_len;
        uint8_t* consume_start;
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (state == stream_empty) {
                debug_log<std::string>("try to consume but find nothing in stream");
                debug_endl();
                return stream_empty;
            }
            consume_start = start;
            if (endp1 > start) {
                consume_len = endp1 - start;
                start = endp1;
            } else {
                consume_len = end_buf - start;
                start = buffer;
            }
        }
        debug_log<std::string>("consume stream... len is ");
        debug_log<stream_size_t>(consume_len);
        debug_endl();
        consume(consume_start, consume_len);
        if (blocked) {
            debug_log<std::string>("consume stream complete\n");
            debug_log<std::string>("update protected zone");
            debug_endl();
            return consumer_update_protected_range();
        }
        debug_log<std::string>("return state ");
        debug_log<stream_state_e>(state);
        debug_log<std::string>("-------------");
        debug_endl();
        return (state);
    };

    template <ISProducer T>
    multithread_stream& operator<<(T& it) {
        producer_mutex.lock();
        mutex.lock();
        const uint8_t* const pstart(protected_start);
        uint8_t* pend(protected_endp1);
        mutex.unlock();

        DEBUG_STREAM(strema_debug_iterator,"stream input...");
        #define EMPTY_SIZE(start, end) ((end < start) ? (start - end) : (stream_size - (end - start)))
        #define CIRCULAR_PLUSPLUS(end) (end = ((++end) == end_buf) ? (start_buf) : end)
        if (it.minmax_len() > EMPTY_SIZE(pstart, pend)) {
            DEBUG_STREAM(strema_debug_iterator, "minmax len bigger than empty size return !!", false);
            // DEBUG_STREAM(strema_debug_iterator, it.minmax_len)
            // std::cout << "empty size is" << EMPTY_SIZE(pstart, pend) << std::endl;
            goto unlock;
        }
        for (; !it.is_ended() && EMPTY_SIZE(pstart, pend) > 1; ++it, CIRCULAR_PLUSPLUS(pend)) {
            // DEBUG_STREAM(strema_debug_iterator, "++")
            *pend = *it;
            // DEBUG_STREAM(strema_debug_iterator, *pend, false);
            // DEBUG_STREAM(strema_debug_iterator, " ", false);
        }
        
        DEBUG_STREAM(strema_debug_iterator, " ");

        mutex.lock();
        if (it.is_ended()) {
            protected_endp1 = pend;
            state = stream_normal;
        } else state = stream_full;
        endp1 = protected_endp1;
        mutex.unlock();
    unlock:
        producer_mutex.unlock();
        #undef EMPTY_SIZE

        return (*this);
    }

    template <ISBlockedConsumer T>
    multithread_stream& operator>>(T& consumer) {
        stream_size_t consume_len;
        const uint8_t* consume_start;
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (state == stream_empty) {
                return (*this);
            }
            consume_start = start;
            if (endp1 > start) {
                consume_len = endp1 - start;
                start = endp1;
            } else {
                consume_len = end_buf - start;
                start = buffer;
            }
        }
        // if (!consume_len) return (*this);
        debug_log<std::string>("consume stream... len is ");
        debug_log<stream_size_t>(consume_len);
        debug_endl();
        consumer.blocked_consume(consume_start, consume_len);
        consumer_update_protected_range();
        debug_log<std::string>("after consume state is: ");
        debug_log<stream_state_e>(state);
        debug_log<std::string>("  -------------");
        debug_endl();
        return (*this);
    }


    // stream_state_e void
   private:
    mutable std::mutex mutex;
    mutable std::mutex producer_mutex;
    uint8_t buffer[stream_size];
    stream_state_e state;
    uint8_t* start;
    uint8_t* endp1;
    uint8_t* protected_start;
    uint8_t* protected_endp1;

    uint8_t* const start_buf;
    uint8_t* const end_buf;

    uint8_t* circular_ptr_add(stream_size_t len) const {
        // std::lock_guard<std::mutex> lock(mutex);
        if (endp1 + len < end_buf)
            return endp1 + len;
        else if (endp1 + len == end_buf)
            return start_buf;
        else
            return start_buf + len - (end_buf - endp1);
    }
    void circular_ptr_copy(const uint8_t* data, stream_size_t len) {
        if (!len) return;
        uint8_t* temp = circular_ptr_add(len);
        if (temp < endp1) {
            std::memcpy(endp1, data, end_buf - endp1);
            std::memcpy(start_buf, data + (end_buf - endp1), temp - start_buf);
        } else
            std::memcpy(endp1, data, len);
    }

    stream_size_t empty_size() {
        return (protected_endp1 < protected_start) ? (protected_start - protected_endp1) : (stream_size - (protected_endp1 - protected_start));
    }

};
}  // namespace EasyRobotCommands