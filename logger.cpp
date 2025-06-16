#include "logger.h"

template<Printable T>
std::shared_ptr<metric_manager<T>> logger::getMetric(const std::string &name) {
    if (!containsName(name)) {
        throw std::runtime_error("metric not found");
    }
    return std::static_pointer_cast<metric_manager<T>>(metrics[name]);
}

void logger::removeMetric(const std::string &name) {
    std::unique_lock<std::mutex> lk(metric_data_mutex);
    if (!containsName(name)) {
        std::cerr << name << " does not exist..." << std::endl;
        return;
    }
    metrics.erase(name);
}

void logger::logInfo(const std::string& name_, const std::string &timestamp) noexcept {
    std::unique_lock<std::mutex> lock(busy_mutex, std::try_to_lock);
    if (lock.owns_lock()) {
        std::string to_print = timestamp;
        to_print += " | ";
        for (auto iter = metrics.begin(); iter != metrics.end(); ++iter) {
            std::string name = iter->first;
            if (onUpdatePrintAll || (!onUpdatePrintAll && name_ == name)) {
                to_print += "\"" + name + "\" - ";
                metrics[name]->last_state_mutex.lock();
                to_print += metrics[name]->latest_state;
                metrics[name]->last_state_mutex.unlock();

                if (std::next(iter) != metrics.end())
                    to_print += ", ";
            }
        }
        std::cout << to_print << std::endl;
        write(to_print);
    }
}

void logger::write(std::string txt) {
    outFile << txt << '\n';
}

void logger::startWriteAt(std::string path) {

    outFile.open(path);
    if (!outFile) {
        throw std::runtime_error("can\'t create a file!");
    }
}
