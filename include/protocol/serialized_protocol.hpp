#pragma once
#include <cstdint>
#include <ranges>

#include "container/concepts.hpp"
#include "easy_robot_commands/shared_member/concepts.hpp"
#include "protocol/crc.hpp"

#include "easy_robot_commands/shared_member/base_caller_template.hpp"
#include "easy_robot_commands/base_caller_specified/robot_modules_mode.hpp"
    namespace EasyRobotCommands {
using protocol_size_t = uint16_t;
using protocol_pack_id = uint8_t;
enum protocol_type_e {
    protocol0 = 0,
};

template <ISCRCConfigable crc_config_t, protocol_type_e type>
struct ProtocolConfig {
    using CRC_CONFIG = crc_config_t;
    // static constexpr protocol_pack_id ID = id;
    static constexpr protocol_type_e TYPE = type;
};


template <typename protocol_config_t>
concept ISProtocolConfigable = requires {
    ISCRCConfigable<typename protocol_config_t::CRC_CONFIG>;
    // { protocol_config_t::ID } -> std::convertible_to<protocol_pack_id>;
    { protocol_config_t::TYPE } -> std::convertible_to<protocol_type_e>;
};

template <typename T>
concept Packable = requires(T& a) {
    has_get_structure_data_method<T>;
    has_get_struct_data_method<T>;
    typename T::struct_data_t;
    { T::struct_data_t::ID } -> std::common_with<protocol_pack_id>;
};

inline uint8_t need_escape(uint8_t b) {
    if (b == 0x7d || b == 0x7e || b == 0x7f) return 1;
    return 0;
}
inline uint8_t escape(uint8_t b) {
    return b - 0x7d;
}

inline size_t escape_data(uint8_t* data, size_t len, uint8_t* buffer) {
    size_t i = 0;
    for (uint8_t* p = data ; p < data + len; p++, i++) {
        if (!need_escape(*p)) *(buffer + i) = *p;
        else {
            *(buffer + i) = 0x7f;
            ++i;
            *(buffer + i) = escape(*p);
        }
    }
    return i;
}

template <Packable T, ISProtocolConfigable configT>
class PackGenerator /*requires ISProducer*/ {
   public:
    PackGenerator(const T& src_) : temp(pack_start),
                                   is_ended_(false),
                                   is_escaping(false),
                                   crc16(configT::CRC_CONFIG::CRC_INIT),
                                   index(-2),
                                   src(src_),
                                   structure_data((const uint8_t*)&src_.get_structure_data()),
                                   structure_data_size(sizeof(src_.get_structure_data())),
                                   minmax_len_(structure_data_size + 5) {
        static_assert(configT::TYPE == protocol0);
        // static_assert(structure_data_size);
    }

    PackGenerator& operator++() {
        // std::cout << "++ op: index  :"<< std::setw(5) << index << std::endl;
        ++index;
        if (index == -1) {
            temp = T::struct_data_t::ID;
            // std::cout << "ID field: " << std::setw(5) << "0x" << std::hex << static_cast<int>(temp) << std::dec <<std::endl;
            goto crc_calc;
        } else if (index >= structure_data_size) {
            static int num;
            if (index == structure_data_size) {
                // std::cout << std::hex << "crc16 is: " << crc16 << std::dec << std::endl;
                num = escape_data((uint8_t*)&crc16, 2, temp_crc);
                temp_crc[num++] = pack_end;
            }
            if (--num == -1) is_ended_ = true;
            temp = temp_crc[index - structure_data_size];
            goto end;
        } else {
            if (structure_data[index] == pack_start ||
                structure_data[index] == pack_end ||
                structure_data[index] == pack_escape) {
                enter_escaping();
                goto end;
            }
            if (is_escaping) {
                exit_escaping();
                goto end;
            }
            // normal calc crc 
            temp = structure_data[index];
        }
    crc_calc:
        crc16 = CRC16<typename configT::CRC_CONFIG>::modbus_calc_one(&temp, crc16);
    end:
        if (!is_ended_) {
            // std::cout << "*it: 0x" << std::hex <<  static_cast<int>(temp) << std::dec << std::endl;
        }
        return (*this);
    }

    bool is_ended() const {
        return is_ended_;
    }

    protocol_size_t minmax_len() const {
        // std::cout << "minmax len is: " << minmax_len_ << std::endl;
        return minmax_len_;
    }

    uint8_t operator*() const {
        return temp;
    }

    void regenerate_init() {
        temp = pack_start;
        is_ended_ = false;
        is_escaping = false;
        crc16 = configT::CRC_CONFIG::CRC_INIT;
        index = -2;
        minmax_len_ = structure_data_size + 5;
    }

   private:
    static constexpr uint8_t pack_start = 0x7d;
    static constexpr uint8_t pack_end = 0x7e;
    static constexpr uint8_t pack_escape = 0x7f;

    uint8_t temp;
    uint8_t temp_crc[5];
    bool is_ended_;
    bool is_escaping;
    uint16_t crc16;
    int index;
    const T& src;
    const uint8_t* structure_data;
    const protocol_size_t structure_data_size;
    protocol_size_t minmax_len_;

    void enter_escaping() {
        is_escaping = true;
        temp = pack_escape;
        crc16 = CRC16<typename configT::CRC_CONFIG>::modbus_calc_one(&structure_data[index], crc16);
        --index;
        ++minmax_len_;
    }

    void exit_escaping() {
        is_escaping = false;
        temp = structure_data[index] - pack_escape;
    }
};

}  // namespace EasyRobotCommands