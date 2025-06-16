#include "metric_manager.h"
#include <chrono>


std::string strtimestamp() {
    auto now = std::chrono::system_clock::now();
    using namespace std::chrono;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) % 1000;
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
        << ':' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}


void BETTERCALLSAUL(const std::string& name) {
    // 4 ночи, я дебажу изо всех сил и у меня послезавтра экз :)))
    logger::instance().logInfo(name, strtimestamp());
}
