#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>

namespace EasyRobotCommands {

#define STREAM_DEBUG 0

template <typename T1>
inline void debug_log(T1 mes) {
    if (STREAM_DEBUG) {
        std::cout << mes;
    }
}

inline void debug_endl() {
    if (STREAM_DEBUG) {
        std::cout << std::endl;
    }
}
using stream_size_t = uint16_t;

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

    class Iterator {
        using self = Iterator;
        using pointer = uint8_t*;
        using value_t = uint8_t;
        using ref = uint8_t&;

       public:
        Iterator(const multithread_stream<stream_size>& stream_, stream_size_t index_) : stream(stream_), index(index_) {}
        bool operator!=(const Iterator& other) const {
            return index != other.index;
        }
        Iterator& operator++() {
            index = (index + 1) % stream_size;
            return *this;
        }
        uint8_t operator*() const {
            return this->stream.buffer[index];
        }

       private:
        const multithread_stream<stream_size>& stream;
        stream_size_t index;
    };

    Iterator data_begin() { return Iterator(*this, protected_start - buffer); }
    Iterator data_end() { return Iterator(*this, protected_endp1 - buffer); }
    Iterator empty_begin() { return data_end(); }
    Iterator empty_end() { return data_begin(); }

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
        return (protected_endp1 < protected_start) ? 
            (protected_start - protected_endp1) : 
            (stream_size - (protected_endp1 - protected_start));
    }
};
}  // namespace EasyRobotCommands