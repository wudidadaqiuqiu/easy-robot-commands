#pragma once
#include <cstdint>
#include <ranges>
#include "protocol/crc.hpp"

namespace EasyRobotCommands {
using protocol_size_t = uint16_t;
// using protocol_pack_id = uint8_t;
enum protocol_type_e {
    protocol1,
};

template<ISCRCConfigable crc_config_t, protocol_type_e type>
struct  ProtocolConfig {
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


template<ISProtocolConfigable config_t>
void protocol_pack(uint8_t* src, uint8_t* dst, protocol_size_t len) {
    // CRC16<typename config_t::CRC_CONFIG>::modbus_calc();
    if (!(src && dst)) return;
    
}

}