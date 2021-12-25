#include "timer.h"
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

const std::chrono::duration<double> Timer::buffer{0.05};

tm get_tm() {
    time_t rawtime;
    time (&rawtime);
    tm timeinfo;
    gmtime_s(&timeinfo, &rawtime);
    return timeinfo;
}

int Timer::year() {
    return get_tm().tm_year + 1900;
}
int Timer::month() {
    return get_tm().tm_mon + 1;
}
int Timer::day() {
    return get_tm().tm_mday;
}
int Timer::weekday() {
    return get_tm().tm_wday + 1;
}
int Timer::hour() {
    return get_tm().tm_hour;
}
int Timer::minute() {
    return get_tm().tm_min;
}
int Timer::second() {
    return get_tm().tm_sec;
}
int Timer::millisecond() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 1000;
}

std::string pad(int num, int digits) {
    std::string result = std::to_string(num);
    result.insert(result.begin(), digits - result.size(), '0');
    return result;
}

std::string Timer::iso_time() {
    return pad(year(), 4) + '-' + pad(month(), 2) + '-' + pad(day(), 2) + 
        'T' + pad(hour(), 2) + ':' + pad(minute(), 2) + ':' + pad(second(), 2) + 'Z';
}

void Timer::start() {
    if (!running) {
        m_start = std::chrono::high_resolution_clock::now();
    }
    running = true;
}

void Timer::stop() {
    if (running) {
        m_end = std::chrono::high_resolution_clock::now();
    }
    running = false;
}

void Timer::reset() {

}

double Timer::elapsed() {
    if (running) {
        m_end = std::chrono::high_resolution_clock::now();
    }
    return std::chrono::duration<double>(m_end - m_start).count();
}

bool Timer::isRunning() {
    return running;
}

void Timer::sleep(double duration, std::chrono::high_resolution_clock::time_point start) {
    std::chrono::duration<double> duration_t{duration};

    if (duration_t > buffer) {
        std::this_thread::sleep_for(duration_t - buffer);
    }
    
    while (std::chrono::high_resolution_clock::now() < start + duration_t);
}