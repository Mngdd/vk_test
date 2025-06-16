#include "logger.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <random>
#include <chrono>
#include <atomic>

logger &lg = logger::instance();
std::atomic<bool> running(true);

// шегерируем случ числа
void generator() {
    int rand_num = 0;
    auto metric_ptr = lg.addMetric("CPU", &rand_num);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);

    while (running) {
        {
            rand_num = dis(gen);
            //std::cout << "GEN: new num = " << rand_num << std::endl;
        }

        metric_ptr->updateInfo();

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

// Поток, который прибавляет случайное число (1-5) каждую секунду
void incrementer() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 5);

    int local_sum = 0;
    auto metric_ptr = lg.addMetric("HTTP requests RPS", &local_sum, true);

    while (running) {
        for (int i=0; i<dis(gen); i++) {
            int random_add = dis(gen);
            local_sum += random_add;
        }

        metric_ptr->updateInfo();

        std::cout << local_sum << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        throw std::logic_error("required 1 argument: path to file...");
    }
    lg.startWriteAt(argv[1]);

    //lg.setPrintAllOnUpdate(false);
    std::thread t1(generator);
    std::thread t2(incrementer);

    // Ожидаем 10 секунд, затем завершаем потоки
    std::this_thread::sleep_for(std::chrono::seconds(10));
    running = false;

    t1.join();
    t2.join();
}
