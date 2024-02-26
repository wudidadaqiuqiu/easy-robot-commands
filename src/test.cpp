#include <iostream>

#include "container/multithread_stream.hpp"
#include "protocol/crc.hpp"
#include "protocol/serialized_protocol.hpp"
// #include "easy_robot_commands/robot_modules_mode.hpp"
#include "easy_robot_commands/base_caller_specified/robot_modules_mode.hpp"
#include "easy_robot_commands/shared_member/concepts.hpp"
#include "easy_robot_commands/stream.hpp"

int main() {
    using easy_robot_commands::msg::RobotModulesMode;
    using EasyRobotCommands::CRC16;
    using EasyRobotCommands::CRC16Config;
    using EasyRobotCommands::ea_base_caller;
    using EasyRobotCommands::multithread_stream;
    using EasyRobotCommands::protocol_type_e;
    using EasyRobotCommands::ProtocolConfig;
    // EasyRobotCommands::robot_modules_mode module_mode{};
    // std::cout << EasyRobotCommands::robot_modules_mode::my_class_name;
    ea_base_caller<RobotModulesMode> m{};
    std::cout << EasyRobotCommands::StructDataT<RobotModulesMode>::topic_name << std::endl;
    std::cout << "sizeof struct: " << sizeof(EasyRobotCommands::StructDataT<RobotModulesMode>) << std::endl;
    // std::cout << ""
    EasyRobotCommands::Stream<20, ProtocolConfig<CRC16Config<0xFFFF, 0x1021>, protocol_type_e::protocol0>> s;
    s.triggered_from(m);
    // s.begin();
    // (void)s;

    using Config = CRC16<CRC16Config<0xFFFF, 0x1021>>;
    static constexpr auto table = Config::get_table();
    (void) table;
    // Print the generated CRC-16 table
    // std::cout << "CRC-16 Table: ----------------------\n\n";
    // for (const auto& element : table) {
    //     std::cout << element << " ";
    // }
    // std::cout << "\n\n" << std::endl;

    // uint8_t temp[2] = { 0x11, 0x11};
    // uint16_t crc_temp = 0;
    // std::cout << "my constructed crc is: " << ((crc_temp = Config::modbus_calc(temp, 2)))
    //     << " hex:" << std::hex  << crc_temp << std::dec << std::endl;
    // EasyRobotCommands::protocol_pack<ProtocolConfig<CRC16Config<0xFFFF, 0x1021>, protocol_type_e::protocol1>>(nullptr, nullptr, 1);

    // multithread_stream<10> a;
    // multithread_stream<10>::Iterator p = a.data_begin();
    // (void)p;
}