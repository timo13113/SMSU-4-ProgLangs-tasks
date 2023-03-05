#include "libs.hpp"
#include "classes.hpp"
#include "readf.hpp"
#include "tui.hpp"
#include "debug.hpp"

std::vector<std::string> city_names; // названия городов
std::vector<std::string> transport_names; // названия транспорта
std::vector<bool> transport_whitelist; // разрешенный транспорт
std::vector<std::vector<std::pair<unsigned int /* направление */, std::set<Cruise/* набор ребер из А в Б */>>>> graph;


int main(int argc, char** argv) {
    setlocale(LC_ALL,"ru_RU.UTF-8");  
    EnableRuSymbols;
    // std::cout << __cplusplus;

    read_data("data.txt", city_names, transport_names, transport_whitelist, graph);

    init_tui(city_names, transport_names, transport_whitelist, graph);

    // быстрый запуск внутри командной строки (по кодам городов, без управления транспортом)
    // if (argc > 1)
    // {
    //     debug_mode(argc, argv);
    //     return 0;
    // }

    // log();

    return 0;
}