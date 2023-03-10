#include "libs.hpp"
#include "classes.hpp"
#include "readf.hpp"
#include "tui.hpp"
#include "debug.hpp"

// названия городов
std::vector<std::string> city_names;
// названия транспорта
std::vector<std::string> transport_names;
// разрешенный транспорт
// std::vector<bool> transport_whitelist;
// граф
Graph graph;
// std::vector<std::vector<std::pair<unsigned int, std::set<Cruise>>>> graph;


int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    EnableRuSymbols;
    // std::cout << __cplusplus;

    if (argc < 2) {
        std::cout << "Не предоставлено имя файла, из которого выгружаются данные!\n";
        return 0;
    }


    auto t_read_data = timeit_void(read_data, "read_data");

    // "data.txt" - <50 строк
    // "big_data.txt" - 10000 строк 1000 городов
    // "very_big_data.txt" - 100000 строк 1000 городов
    t_read_data(argv[1], city_names, transport_names, graph);

    print_memory();
    init_tui(city_names, transport_names, graph);
    print_memory();

    return 0;
}
