#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <algorithm>
#include <optional>
#include <chrono>
#include <functional>

#define SHOW_TRIVIAL_PATHS true

#ifdef _WIN32
    #include <pdcurses.h>

    #include <windows.h>
    #include <psapi.h>
    #define EnableRuSymbols SetConsoleCP(65001); SetConsoleOutputCP(65001)

    void print_memory(std::fstream &log) {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);

        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

        log << "currently used phys memory: " << pmc.WorkingSetSize
            << " out of " << memInfo.ullTotalPhys << " total\n";
        log << "currently used phys memory: " << pmc.WorkingSetSize/(1024*1024)
            << " MB out of " << memInfo.ullTotalPhys/(1024*1024) << " MB total\n";
        log << "peak used phys memory: " << pmc.PeakWorkingSetSize
            << " out of " << memInfo.ullTotalPhys << " total\n";
        log << "peak used phys memory: " << pmc.PeakWorkingSetSize/(1024*1024)
            << " MB out of " << memInfo.ullTotalPhys/(1024*1024) << " MB total\n";
    }
#else
    #include <ncurses.h>
    #include <sys/resource.h>
    #define EnableRuSymbols ;
    void print_memory(std::fstream &log) {
        struct rusage mem;
        getrusage(RUSAGE_SELF, &mem);
        log << "peak used phys memory: " << mem.ru_maxrss << " KB" << std::endl;
        return;
    }
#endif

/**
 * Функция замера времени.
 * 
 * @param func функция, работу которой нужно замерить (возвращает любой тип, кроме void)
 * @param name название функции для логов
 * @return функцию, аналогичную аргументу, но которая выведет в логах/на экран время работы
 */
template<typename F>
auto timeit_not_void(const F& func, const char* name, std::fstream &log) {
    return [&func, name, &log](auto&&... args){
        using ret_type = decltype(func(std::forward<decltype(args)>(args)...));
        const auto start_time = std::chrono::high_resolution_clock::now();
        ret_type res = func(std::forward<decltype(args)>(args)...);
        const auto end_time = std::chrono::high_resolution_clock::now();
        const auto time = end_time - start_time;
        log << name << " took " << time/std::chrono::microseconds(1) << " us to run\n";
        return res;
    };
}
/**
 * Функция замера времени.
 * 
 * @param func функция, работу которой нужно замерить (возвращает void)
 * @param name название функции для логов
 * @return функцию, аналогичную аргументу, но которая выведет в логах/на экран время работы
 */
template<typename F>
auto timeit_void(const F& func, const char* name, std::fstream &log) {
    return [&func, name, &log](auto&&... args){
        const auto start_time = std::chrono::high_resolution_clock::now();
        func(std::forward<decltype(args)>(args)...);
        const auto end_time = std::chrono::high_resolution_clock::now();
        const auto time = end_time - start_time;
        log << name << " took " << time/std::chrono::microseconds(1) << " us to run\n";
    };
}
