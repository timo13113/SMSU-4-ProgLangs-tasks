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

    auto t_read_data = timeit_void(read_data, "read_data");

    // t_read_data("data.txt", city_names, transport_names, transport_whitelist, graph); // <50 строк
    // t_read_data("big_data.txt", city_names, transport_names, transport_whitelist, graph); // 10000 строк 1000 городов
    t_read_data("very_big_data.txt", city_names, transport_names, transport_whitelist, graph); // 100000 строк 1000 городов

    print_memory();
    init_tui(city_names, transport_names, transport_whitelist, graph);
    print_memory();

    return 0;
}