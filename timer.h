#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>

class Timer {

    static const std::chrono::duration<double> buffer;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_end;

    bool running = false;

public:

    static int year();
    static int month();
    static int day();
    static int weekday();
    static int hour();
    static int minute();
    static int second();
    static int millisecond();
    static std::string iso_time();

    void start();
    void stop();
    void reset();
    bool isRunning();
    double elapsed();

    static void sleep(double duration, std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now());

};

#endif