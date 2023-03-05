#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <algorithm>
#include <optional>
#include <windows.h>

#ifdef _WIN32
    #include <windows.h>
    #define EnableRuSymbols SetConsoleCP(65001); SetConsoleOutputCP(65001)
    #include <pdcurses.h>
#else
    #include <ncurses.h>
#endif


#define V(A) std::vector<A>
#define P(A, B) std::pair<A, B>