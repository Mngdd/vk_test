#ifndef METRIC_MANAGER_H
#define METRIC_MANAGER_H
#include <string>
#include <utility>
#include "logger.h"
#include "fair_mutex.hpp"
#include "concepts.hpp"
#include <thread>

void BETTERCALLSAUL(const std::string& name);

template<Printable T>
std::string valueToString(const T &value) {
    std::ostringstream oss;
    oss << value; // Записываем значение в поток
    return oss.str(); // Преобразуем поток в строку
}

class logger;



// фейковый базовый класс чтоб в мапу запихнуть
struct Base {
    virtual ~Base() = default;

    virtual void updateInfo() = 0;

    fair_mutex last_state_mutex;
    std::string latest_state = "no data";
};


template<Printable T>
class metric_manager : public Base {
    T *value_ptr; // read-only  (указател на коснстантина)
    const std::string name;
    bool is_cumulative;
    // если кумулятивное, то возвращаем значение к этому дефолтному
    const T default_value;

public:
    // будет странно если две абсолютно одинаковые функции работают с одним и тем же числом
    metric_manager() = delete;

    metric_manager(const metric_manager &) = delete;

    metric_manager &operator=(const metric_manager &) = delete;

    metric_manager(const metric_manager &&) = delete; // переносить можно
    metric_manager(metric_manager &&) = delete; // переносить можно

    metric_manager(std::string name, T *vp, bool cu, const T dv) : value_ptr(vp), name(std::move(name)),
                                                                         is_cumulative(cu), default_value(dv) {}

    ~metric_manager() = default;

    // фиксирует значения и передает их в threadedEnroller
    void updateInfo() {
        std::thread t([this]() { this->threadedEnroller(); });
        t.detach();
    }


    // ждет возможности записать их в очередь логгера и умирает
    void threadedEnroller() {
        last_state_mutex.lock();
        latest_state = valueToString(*value_ptr);
        if (is_cumulative)
            *value_ptr = default_value;

        std::thread t(BETTERCALLSAUL, name);
        t.detach();

        last_state_mutex.unlock();
    }
};


#endif //METRIC_MANAGER_H
