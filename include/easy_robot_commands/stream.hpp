#ifndef EASY_ROBOT_COMMANDS_STREAM_HPP
#define EASY_ROBOT_COMMANDS_STREAM_HPP

#include <vector>
#include <cstdint>
namespace EasyRobotCommands {
class MutexStream {
   private:
   std::vector<uint8_t> data;
   std::vector<uint8_t> temp;
    /* data */
   public:
    void put(std::vector<uint8_t>& a) {
        (void)a;
    };
    std::vector<uint8_t>& pop(int len) {
        if (len <= 0) return temp;
        return temp;
    }
    MutexStream() : data(), temp() {};
};

}  // namespace EasyRobotCommands
#endif