#ifndef EASY_ROBOT_COMMANDS__SHARED_MEMBER__TRIGGER_OPERATION_HPP
#define EASY_ROBOT_COMMANDS__SHARED_MEMBER__TRIGGER_OPERATION_HPP

#include <functional>
#include <mutex>

namespace EasyRobotCommands {

template <typename call_class>
class trigger_operation {
private:
    mutable std::mutex mutex;
    std::function<void(call_class&)> operate;
public:
    trigger_operation(const std::function<void(call_class&)>& operation) : operate(operation) {};
    // trigger_operation() : operate([](call_class){}){};
    void triggered(call_class& caller) const {
        std::lock_guard<std::mutex> lock(mutex);
        operate(caller);
    };
    void change_operation(const std::function<void(call_class&)>& operation) {
        std::lock_guard<std::mutex> lock(mutex);
        operate = operation;
    };
    const std::function<void(call_class&)>& get_operation() const {
        std::lock_guard<std::mutex> lock(mutex);
        return operate;
    };
};

} // namespace EasyRobotCommands

#endif
