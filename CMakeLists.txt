cmake_minimum_required(VERSION 3.5)
project(metric_management)
set(CMAKE_CXX_STANDARD 23)

set(SOURCE
        main.cpp
)
set(LIB
        logger.cpp
        metric_manager.cpp
        fair_mutex.hpp
        logger.h
        metric_manager.h

)

add_library(manager_lib STATIC ${LIB})

add_executable(main ${SOURCE})

target_link_libraries(main manager_lib)
