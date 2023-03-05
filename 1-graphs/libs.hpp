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


#ifdef _WIN32
    #include <pdcurses.h>

    #include <windows.h>
    #include <psapi.h>
    #define EnableRuSymbols SetConsoleCP(65001); SetConsoleOutputCP(65001)
    
    void print_memory() {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);

        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

        std::cout << "currently used phys memory: " << pmc.WorkingSetSize << " out of " << memInfo.ullTotalPhys << " total\n";
        std::cout << "currently used phys memory: " << pmc.WorkingSetSize/(1024*1024) << " MB out of " << memInfo.ullTotalPhys/(1024*1024) << " MB total\n";
        std::cout << "peak used phys memory: " << pmc.PeakWorkingSetSize << " out of " << memInfo.ullTotalPhys << " total\n";
        std::cout << "peak used phys memory: " << pmc.PeakWorkingSetSize/(1024*1024) << " MB out of " << memInfo.ullTotalPhys/(1024*1024) << " MB total\n";
    }
#else
    #include <ncurses.h>
    #define EnableRuSymbols ;
    void print_memory() { return; }
#endif

template<typename F> 
auto timeit_not_void(const F& func, const char* name) // функция таймера оборачивает функцию-аргумент
{
    return [func, name](auto&&... args)
    {
        using ret_type = decltype(func(std::forward<decltype(args)>(args)...));
        const auto start_time = std::chrono::high_resolution_clock::now();
        ret_type res = func(std::forward<decltype(args)>(args)...); // исполнение функции
        const auto end_time = std::chrono::high_resolution_clock::now();
        const auto time = end_time - start_time;
        std::cout << name << " took " << time/std::chrono::microseconds(1) << " us to run\n";
        return res;
    };
}
template<typename F> 
auto timeit_void(const F& func, const char* name) // функция таймера оборачивает функцию-аргумент
{
    return [func, name](auto&&... args) 
    {
        const auto start_time = std::chrono::high_resolution_clock::now();
        func(std::forward<decltype(args)>(args)...);
        const auto end_time = std::chrono::high_resolution_clock::now();
        const auto time = end_time - start_time;
        std::cout << name << " took " << time/std::chrono::microseconds(1) << " us to run\n";
    };
}
