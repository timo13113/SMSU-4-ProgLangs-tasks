#include "libs.hpp"
#include "classes.hpp"
#include "readf.hpp"
#include "tui.hpp"
#include "debug.hpp"

// названия городов
std::vector<std::string> city_names;
// названия транспорта
std::vector<std::string> transport_names;
// граф
Graph graph;



int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    EnableRuSymbols;
    // std::cout << __cplusplus;

    if (argc < 2) {
        std::cout << "Не предоставлено имя файла, из которого выгружаются данные!\n";
        return 0;
    }

    std::fstream log;
    log.open("log.txt", std::fstream::out);
    log << "starting program...\n";

    print_memory(log);
    auto t_read_data = timeit_void(read_data, "read_data", log);

    // "data.txt" - <50 строк
    // "big_data.txt" - 10000 строк 1000 городов
    // "very_big_data.txt" - 100000 строк 1000 городов
    t_read_data(argv[1], city_names, transport_names, graph, log);

    print_memory(log);
    init_tui(city_names, transport_names, graph, log);
    print_memory(log);

    log << "finishing program...\n";
    log.close();
    return 0;
}
