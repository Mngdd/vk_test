#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <ctime>
#include <map>
#include <string>
#include <iostream>
#include <mutex>
#include <memory>
#include <fstream>
#include "metric_manager.h"
#include "concepts.hpp"


struct Base;

template<Printable T>
class metric_manager;


class logger {
    // имя; указатель на метрику
    std::map<std::string, std::shared_ptr<Base> > metrics;
    // при обновлении печатать всё или только изменившуюся метрику
    bool onUpdatePrintAll = true;
    // захватываем мьютекс если начинаем запись значений (получив сигнал от какой-то метрики)
    std::mutex busy_mutex;
    std::mutex metric_data_mutex;
    std::ofstream outFile;

    // деструктор приватный тк смысл удалять его нулевой...
    logger() = default;

    ~logger() = default;

    logger(const logger &) = delete;

    logger &operator=(const logger &) = delete;

    logger(logger &&) = delete;

    logger &operator=(logger &&) = delete;

    friend void BETTERCALLSAUL(const std::string& name);
    // обновить значение + логируем его
    void logInfo(const std::string& name_, const std::string &timestamp) noexcept;

    void write(std::string txt);
public:
    void startWriteAt(std::string path);

    // синглтон
    static logger &instance() {
        static logger inst;
        return inst;
    }

    bool containsName(const std::string &name) const {
        return metrics.contains(name);
    }

    template<Printable T>
    std::shared_ptr<metric_manager<T> > addMetric(const std::string &name, T *initValue,
                                                  bool is_cumulative=false, T default_value=T()) {
        std::unique_lock<std::mutex> lk(metric_data_mutex);
        if (containsName(name)) {
            std::cerr << name << " already exists, returning current metric..." << std::endl;
        } else {
            metrics[name] = std::make_shared<metric_manager<T>>(name, initValue, is_cumulative, default_value);
        }
        return std::static_pointer_cast<metric_manager<T> >(metrics[name]);
    };

    void removeMetric(const std::string &name);

    template<Printable T>
    std::shared_ptr<metric_manager<T> > getMetric(const std::string &name);

    // переключить режим печати
    void setPrintAllOnUpdate(const bool &flag) {
        onUpdatePrintAll = flag;
    }
};


#endif
