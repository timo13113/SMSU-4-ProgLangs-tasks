#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <algorithm>
#include <optional>

// заменить строку ниже чтобы перекомпилировать с разными портами ncurses'а
#include <pdcurses.h>

#include "classes.hpp"

std::set<std::pair<unsigned int /* из */, std::pair<unsigned int /* в */, Cruise/* ребро */>>> all_cruises;

std::vector<std::vector<std::pair<unsigned int /* направление */, std::set<Cruise/* набор ребер из А в Б */>>>> graph;

std::vector<std::string> cities;
std::vector<std::string> transport_types;

std::vector<bool> transport_whitelist; // разрешенный транспорт

std::vector<std::vector<Path>> paths_to; // кратчайшие пути из from в город по индексу
std::vector<bool> used; // прошли этот город или нет

bool dijkstra(
        unsigned int from, // город из которого идем
        unsigned int to, // город в который идем
        bool time_over_money = true, // считаем ли мы приорететнее время поверх денег
        bool keep_unefficient_paths = false, // учитывать ли пути, оптимальные по главной переменной, но не оптимальные по второстепенной переменной
        bool count_only_number_of_cities = false, // для задания №3, когда нужно посчитать минимальное количество пересадок
        bool have_limit = false, // если у нас есть предел на количество денег/времени, которое мы можем использовать
        long long unsigned int limit = 0,
        bool logs = false
    ) {
    if (logs) 
        std::cout << "dijkstra started from " << cities[from] << " to " << cities[to] << "\n";
    unsigned int n = cities.size();
    paths_to.clear();
    paths_to.resize(n);
    paths_to.at(from) = std::vector<Path>(1, Path(std::vector<Cruise>(), from, std::vector<unsigned int>())); // тривиальный путь из себя в себя который не требует транспорта и имеет (0, 0) стоимость
    used.clear();
    used.resize(n);
    bool halt_on_limit = false;
    bool halt_condition;
    if (have_limit)
        halt_condition = !halt_on_limit;
    else 
        halt_condition = !used[to];
    if (count_only_number_of_cities) { // если мы минимизируем количество пересадок
        while (halt_condition) // пока не дошли куда надо или пока не превысили лимит
        {
            std::optional<unsigned int> v; // начальная вершина
            for (unsigned int j = 0; j < n; j++)
                if (!paths_to.at(j).empty()) // если есть пути в вершину
                    if (!used.at(j) && (!v.has_value() || paths_to.at(j).at(0).length() < paths_to.at(*v).at(0).length())) // найти неисследованную вершину с минимальными затратами до нее
                        v = j;
            if (!v.has_value() || paths_to.at(*v).at(0).time_cost == UINT64_MAX) // если мы исследовали все вершины но не дошли куда надо ИЛИ мы вынуждены начинать с вершины в которую мы никогда не заходили
            ///////////////////////////////////////////////
            // TODO - нормально обработать ситуацию выше //
            ///////////////////////////////////////////////
            {
                // std::cout << "!!! no path from A to B !!!\n";
                return 1; // то мы не можем прийти к этой вершине вовсе
            }
            if (have_limit && paths_to.at(*v).at(0).length() > limit) // если мы считаем предел и он превышен
            {
                halt_on_limit = true;
                break;
            }
            
            used[*v] = true; // исследовать ее
            for (auto edges: graph.at(*v)) // для каждого набора круизов из нее
            {
                for (auto cruise: edges.second) // для каждого круиза в город под номером edges.first
                {
                    if ((paths_to.at(edges.first).empty() || paths_to.at(*v).at(0).length() + 1 < paths_to.at(edges.first).at(0).length()) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда быстрее и по разрешенному транспорту
                    {
                        std::vector<Path> newpaths; // все новые пути через v в edges.first
                        for (auto x: paths_to.at(*v)) {
                            newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                        }
                        paths_to.at(edges.first) = newpaths; // вставить новые пути в paths_to
                    }
                    else if ((paths_to.at(edges.first).empty() || paths_to.at(*v).at(0).length() + 1 == paths_to.at(edges.first).at(0).length()) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда так же быстро и по разрешенному транспорту
                    {
                        for (auto x: paths_to.at(*v)) {
                            paths_to.at(edges.first).push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                        }
                    }
                }
            }
            if (have_limit)
                halt_condition = !halt_on_limit;
            else 
                halt_condition = !used[to];
        }
    }
    else if (time_over_money) { // если мы ищем сначала самый быстрый путь, а среди них находим самый дешевый
        while (halt_condition) // пока не дошли куда надо или пока не превысили лимит
        {
            std::optional<unsigned int> v; // начальная вершина
            for (unsigned int j = 0; j < n; j++)
                if (!paths_to.at(j).empty()) // если есть пути в вершину
                    if (!used.at(j) && (!v.has_value() || paths_to.at(j).at(0).time_cost < paths_to.at(*v).at(0).time_cost)) // найти неисследованную вершину с минимальными затратами до нее
                        v = j;
            if (!v.has_value() || paths_to.at(*v).at(0).time_cost == UINT64_MAX) // если мы исследовали все вершины но не дошли куда надо ИЛИ мы вынуждены начинать с вершины в которую мы никогда не заходили
            {
                // std::cout << "!!! no path from A to B !!!\n";
                return 1; // то мы не можем прийти к этой вершине вовсе
            }
            // std::cout << v.value();
            if (have_limit && paths_to.at(*v).at(0).time_cost > limit) // если мы считаем предел и он превышен
            {
                halt_on_limit = true;
                break;
            }
            
            used[*v] = true; // исследовать ее
            // в исследованную вершину есть набор путей (минимум 1 путь), оптимальных по времени как минимум, по времени и деньгам как максимум. если их несколько, то их затраты одинаковы по главной или по обеим переменным
            for (auto edges: graph.at(*v)) // для каждого набора круизов из нее
            {
                for (auto cruise: edges.second) // для каждого круиза в город под номером edges.first
                {
                    if ((paths_to.at(edges.first).empty() || paths_to.at(*v).at(0).time_cost + cruise.cruise_time < paths_to.at(edges.first).at(0).time_cost) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда быстрее и по разрешенному транспорту
                    {
                        std::vector<Path> newpaths; // все новые пути через v в edges.first
                        for (auto x: paths_to.at(*v)) {
                            newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                        }
                        paths_to.at(edges.first) = newpaths; // вставить новые пути в paths_to
                    }
                    else if ((paths_to.at(edges.first).empty() || paths_to.at(*v).at(0).time_cost + cruise.cruise_time == paths_to.at(edges.first).at(0).time_cost) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда так же быстро и по разрешенному транспорту
                    {
                        if (keep_unefficient_paths || paths_to.at(*v).at(0).money_cost + cruise.cruise_fare == paths_to.at(edges.first).at(0).money_cost) // если можем прийти так же дешево (или если нам все равно насколько дешево)
                        {
                            for (auto x: paths_to.at(*v)) {
                                paths_to.at(edges.first).push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                            }
                        }
                        else if (paths_to.at(*v).at(0).money_cost + cruise.cruise_fare < paths_to.at(edges.first).at(0).money_cost) // если мы можем прийти дешевле
                        {
                            std::vector<Path> newpaths; // все новые пути через v в edges.first
                            for (auto x: paths_to.at(*v)) {
                                newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                            }
                            paths_to.at(edges.first) = newpaths; // вставить новые пути в paths_to
                        }
                    }
                }
            }
            if (have_limit)
                halt_condition = !halt_on_limit;
            else 
                halt_condition = !used[to];
        }
    }
    else { // если мы ищем сначала самый дешевый путь, а среди них находим самый быстрый
        while (halt_condition) // пока не дошли куда надо или пока не превысили лимит
        {
            std::optional<unsigned int> v; // начальная вершина
            for (unsigned int j = 0; j < n; j++)
                if (!paths_to.at(j).empty()) // если есть пути в вершину
                    if (!used.at(j) && (!v.has_value() || paths_to.at(j).at(0).money_cost < paths_to.at(*v).at(0).money_cost)) // найти неисследованную вершину с минимальными затратами до нее
                        v = j;
            if (!v.has_value() || paths_to.at(*v).at(0).money_cost == UINT64_MAX) // если мы исследовали все вершины но не дошли куда надо ИЛИ мы вынуждены начинать с вершины в которую мы никогда не заходили
            {
                // std::cout << "!!! no path from A to B !!!\n";
                return 1; // то мы не можем прийти к этой вершине вовсе
            }
            if (have_limit && paths_to.at(*v).at(0).money_cost > limit) // если мы считаем предел и он превышен
            {
                halt_on_limit = true;
                break;
            }
            
            used[*v] = true; // исследовать ее
            // в исследованную вершину есть набор путей (минимум 1 путь), оптимальных по времени как минимум, по времени и деньгам как максимум. если их несколько, то их затраты одинаковы по главной или по обеим переменным
            for (auto edges: graph.at(*v)) // для каждого набора круизов из нее
            {
                for (auto cruise: edges.second) // для каждого круиза в город под номером edges.first
                {
                    if ((paths_to.at(edges.first).empty() || paths_to.at(*v).at(0).money_cost + cruise.cruise_fare < paths_to.at(edges.first).at(0).money_cost) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда быстрее и по разрешенному транспорту
                    {
                        std::vector<Path> newpaths; // все новые пути через v в edges.first
                        for (auto x: paths_to.at(*v)) {
                            newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                        }
                        paths_to.at(edges.first) = newpaths; // вставить новые пути в paths_to
                    }
                    else if ((paths_to.at(edges.first).empty() || paths_to.at(*v).at(0).money_cost + cruise.cruise_fare == paths_to.at(edges.first).at(0).money_cost) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда так же быстро и по разрешенному транспорту
                    {
                        if (keep_unefficient_paths || paths_to.at(*v).at(0).time_cost + cruise.cruise_time == paths_to.at(edges.first).at(0).time_cost) // если можем прийти так же дешево (или если нам все равно насколько дешево)
                        {
                            for (auto x: paths_to.at(*v)) {
                                paths_to.at(edges.first).push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                            }
                        }
                        else if (paths_to.at(*v).at(0).time_cost + cruise.cruise_time < paths_to.at(edges.first).at(0).time_cost) // если мы можем прийти дешевле
                        {
                            std::vector<Path> newpaths; // все новые пути через v в edges.first
                            for (auto x: paths_to.at(*v)) {
                                newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                            }
                            paths_to.at(edges.first) = newpaths; // вставить новые пути в paths_to
                        }
                    }
                }
            }
            if (have_limit)
                halt_condition = !halt_on_limit;
            else 
                halt_condition = !used[to];
        }
    }
    if (logs) {
        std::cout << "total path/s(";
        if (time_over_money)
            std::cout << "t > $";
        else
            std::cout << "$ > t";
        if (keep_unefficient_paths)
            std::cout << ", keeping less effitient paths";
        if (count_only_number_of_cities)
            std::cout << ", #";
        std::cout << "): ";
        if (halt_on_limit) { // если мы считаем все города в пределе, то мы выводим все, в которых были, пока предел не превысился (№4, 5)
            std::cout << "\n";
            for (unsigned int i = 0; i < n; i++) {
                if (used[i])
                {
                    std::cout << cities[i] << ":\n";
                    for (auto x: paths_to.at(i))
                    {
                        x.print();
                    }
                }
            }
        }
        else { // если мы считаем конкретные пути из А в Б (№1, 2, 3)
            std::cout << cities[to] << "\n";
            if (count_only_number_of_cities)
                std::cout << "answer - " << paths_to.at(to).at(0).length() << " cruises minimum\n";
            for (auto x: paths_to.at(to))
            {
                x.print();
            }
        }
        std::cout << "\n";
    }
    return 0; // все хорошо и мы выходим из функции нормально
}

void log() {
    std::cout << "Cities: ";
    for (auto x: cities){
        std::cout << x << " ";
    }
    std::cout << "\nTransports: ";
    for (auto x: transport_types){
        std::cout << x << " ";
    }
    std::cout << "\nGraph:\n";
    int i = 0;
    for (auto c1: graph)
    {
        std::cout << "from " << cities[i++] << "\n";
        for (auto c2: c1)
        {
            std::cout << "\tto " << cities[c2.first] << "\n";
            for (auto crs: c2.second)
            {
                std::cout << "\t\tpath " << transport_types[crs.transport_type_id] << ", " << crs.cruise_time <<  ", " << crs.cruise_fare << "\n";
            }
            std::cout << "\t total paths: " << c2.second.size() << "\n";
        }
    }
}

void read_data(std::string filename) {
    std::ifstream data(filename); // read data from file
    std::string delimiter = "\" \"";

    for (std::string line; std::getline(data, line);) // for each line in file
    {
        if (line.find('#') != std::string::npos) // в строке есть символ комментария
            line.erase(line.find('#'), line.size() - line.find('#'));
        if (line.size() == 0) // строка пустая
            continue;

        std::string from_city = line.substr(0, line.find(delimiter));
        from_city.erase(0, 1); // remove heading '"'
        line.erase(0, line.find(delimiter) + delimiter.length());
        std::string to_city = line.substr(0, line.find(delimiter));
        line.erase(0, line.find(delimiter) + delimiter.length());
        std::string remaining = line.substr(0, line.find(delimiter));
        std::string transport_type = remaining.substr(0, remaining.find(' '));
        transport_type.erase(transport_type.length()-1, 1); // remove trailing '"'
        remaining.erase(0, remaining.find(' ') + 1);
        int cruise_time = std::stoi(remaining.substr(0, remaining.find(' '))); 
        remaining.erase(0, remaining.find(' ') + 1);
        int cruise_fare = std::stoi(remaining);

        unsigned int id_from, id_to, id_transport;

        auto from_index = std::find(cities.begin(), cities.end(), from_city); // попробовать найти в векторе городов новый
        if (from_index == cities.end()) { // если его там нет, то добавить к концу вектора
            cities.push_back(from_city);
            id_from = cities.size() - 1;
        } else {// если есть, то вернуть его индекс в векторе
            id_from = std::distance(cities.begin(), from_index);
        }
        auto to_index = std::find(cities.begin(), cities.end(), to_city);
        if (to_index == cities.end()) {// если его там нет, то добавить к концу вектора
            cities.push_back(to_city);
            id_to = cities.size() - 1;
        } else { // если есть, то вернуть его индекс в векторе
            id_to = std::distance(cities.begin(), to_index);
        }
        auto transport_index = std::find(transport_types.begin(), transport_types.end(), transport_type); // попробовать найти в векторе транспортов новый
        if (transport_index == transport_types.end()) {// если его там нет, то добавить к концу вектора
            transport_types.push_back(transport_type);
            id_transport = transport_types.size() - 1;
        } else { // если есть, то вернуть его индекс в векторе
            id_transport = std::distance(transport_types.begin(), transport_index);
        }

        all_cruises.insert({id_from, {id_to, Cruise(id_transport, cruise_time, cruise_fare)}}); // вставить новый путь в общий граф
    }

    graph.resize(cities.size()); // подогнать граф по размеру чтобы вмещать все города
    transport_whitelist.resize(transport_types.size(), true); // подогнать белый спискок по размеру чтобы вмещать все виды транспорта

    for (auto x: all_cruises) // для всех круизов
    {
        bool was_inserted = false;
        for (auto i = graph[x.first].begin(); i != graph[x.first].end(); ++i ) // для всех круизов из города А
        {
            if (i->first == x.second.first) { // если он идет в город Б
                graph[x.first][std::distance(graph[x.first].begin(), i)].second.insert(x.second.second); // добавить в множество путей из А в Б
                was_inserted = true; // поставить флаг
                break;
            }
        }
        if (!was_inserted) // если круиз не был включен, то значит еще нет записи о путях из А в Б
        {
            graph[x.first].push_back({x.second.first, std::set<Cruise>{x.second.second}}); // записываем, создавая множество путей из А в Б
        }
    }
}

unsigned int city_input(const char* str = "из которого начнете движение:") {
    bool valid = false;
    bool warn = false;
    unsigned int city_id;
    std::string bad_city;
    echo();
    curs_set(1);
    while (!valid) {
        clear();
        printw("%s%s\n\n", "Введите город, ", str);
        if (warn) {
            attron(COLOR_PAIR(2));
            printw("%s - такого города нет в базе!\n", bad_city.c_str());
            attroff(COLOR_PAIR(2));
        }
        char input[200];
        getnstr(input, 200);
        // std::cout << err;
        std::string city(input);
        // std::cout << city << "\n";
        auto city_index = std::find(cities.begin(), cities.end(), city);
        if (city_index == cities.end()) {
            warn = true;
            bad_city = city;
        }
        else {
            city_id = std::distance(cities.begin(), city_index);
            valid = true;
        }
            
    }
    return city_id;
}
long long unsigned int limit_input(const char* str = "деньгам, limit_cost:") {
    bool valid = false;
    bool warn = false;
    long long unsigned int limit;
    std::string bad_limit;
    echo();
    curs_set(1);
    while (!valid) {
        clear();
        printw("%s%s\n\n", "Введите ваш предел по ", str);
        if (warn) {
            attron(COLOR_PAIR(2));
            printw("Введите валидное число!\n");
            attroff(COLOR_PAIR(2));
        }
        char input[200];
        getnstr(input, 200);
        char* endptr;
        limit = strtoull(input, &endptr, 10);
        if (input == endptr)
            warn = true;
        else
            valid = true;
    }
    return limit;
}
void transport_input() {
    // bool valid = false;
    // bool warn = false;
    std::fill(transport_whitelist.begin(), transport_whitelist.end(), true);

    std::vector<std::string> bad_transports;
    std::vector<std::pair<bool, std::string>> good_transports;
    echo(); 
    curs_set(1);
    while (1) {
        clear();
        printw("%s\n\n", "Введите транспорт, по которому вы не будете перемещаться, через запятую c пробелом.\nКогда закончите вводить транспорт, оставьте пустую строку и нажмите Enter.");
        if (good_transports.size()) {
            attron(COLOR_PAIR(3));
            printw("Обработан транспорт: ");
            for (auto x: good_transports) {
                if (x.first) printw("записан %s ", x.second.c_str());
                else printw("удален %s ", x.second.c_str());
            }
            printw("\n");
            attroff(COLOR_PAIR(3));
        }
        good_transports.clear();

        if (bad_transports.size()) {
            attron(COLOR_PAIR(2));
            printw("Не обработан транспорт: ");
            for (auto x: bad_transports) {
                printw("%s ", x.c_str());
            }
            printw("\n");
            attroff(COLOR_PAIR(2));
        }
        bad_transports.clear();
        
        char input[200];
        getnstr(input, 200);
        
        long long unsigned int pos = 0;
        std::string delimiter = ", ";
        std::string input_transports(input);
        if (input_transports.size() == 0) 
            return;
        
        bool was_found = false;
        while ((pos = input_transports.find(delimiter)) != std::string::npos)
        {
            std::string suggested_transport = input_transports.substr(0, pos);
            // find suggested_transport in actual transports
            was_found = false;
            for (unsigned int i = 0; i < transport_types.size(); i++)
            {
                if (transport_types.at(i) == suggested_transport) {
                    transport_whitelist.at(i) = transport_whitelist.at(i) xor true;
                    was_found = true;
                    good_transports.push_back(std::make_pair(transport_whitelist.at(i), suggested_transport));
                    break;
                }
            }
            if (!was_found)
                bad_transports.push_back(suggested_transport);
            
            input_transports.erase(0, pos + delimiter.size());
        }
        // find suggested_transport in actual transports
        was_found = false;
        for (unsigned int i = 0; i < transport_types.size(); i++)
        {
            if (transport_types.at(i) == input_transports) {
                transport_whitelist.at(i) = transport_whitelist.at(i) xor true;
                was_found = true;
                good_transports.push_back(std::make_pair(transport_whitelist.at(i), input_transports));
                break;
            }
        }
        if (!was_found)
            bad_transports.push_back(input_transports);
        
        // for (bool x: transport_whitelist)
        //     std::cout << x;
        // std::cout << "\n";
    }
}

void we_are_sorry(){
        clear();
    std::string truly_sorry1 = 
std::string("Мы не нашли путь между этими городами с такими условиями :(\n\n");
    std::string truly_sorry2 = 
std::string("  _   __  ___ _  ___ __   ___ _   _ _   _  ___  \n") + 
std::string(" | | /  |/ __| |/ / '_ \\ / _ \\ |_| | |_| |/ _ \\ \n") + 
std::string(" | |'/| | (__|   <| |_) |  __/  _  |  _  |  __/ \n") + 
std::string(" |__/ |_|\\___|_|\\_\\ .__/ \\___|_| |_|_| |_|\\___| \n") + 
std::string("                  |_|                           \n") +
std::string("  ___ ___ __  _  ____ _   __   ___  ___  ___ ___   \n") +
std::string(" / __/ _ \\\\ \\| |/ / _` | /  \\ / _ \\/ _ \\/   V   \\  \n") + 
std::string("| (_| (_) /      < (_| |/ /\\ \\  __/  __/  /\\ /\\  \\ \n") +
std::string(" \\___\\___/__/|_|\\_\\__,_/_/  \\_\\___|\\___|_/  V  \\__\\\n");
    attron(COLOR_PAIR(2));
    printw(truly_sorry1.c_str());
    printw(truly_sorry2.c_str());
    attroff(COLOR_PAIR(2));
        printw("\n\nНажмите любую клавишу, чтобы вернуться в главное меню...");
        curs_set(0);
        getch();
}
void path_print(Path path) {
            printw("Путь: %s", cities[path.start_city].c_str());
            for (unsigned int i = 0; i < path.length(); i++)
            {
                std::string cruise_str;
                cruise_str = 
                    "(взяв билет на " + transport_types.at(path[i].second.transport_type_id) + 
                    ", проведя в пути " + std::to_string(path[i].second.cruise_time) + 
                    " минут, потратив "+ std::to_string(path[i].second.cruise_fare) + " рублей)";
                printw(" -> %s -> %s", 
                    cruise_str.c_str(), 
                    cities[path[i].first].c_str()
                );
            }
            attron(COLOR_PAIR(3));
            printw("\nВсего будет потрачено %llu минут, %llu рублей и будет осуществлено %llu прямых рейсов.", path.time_cost, path.money_cost, path.length());
            attroff(COLOR_PAIR(3));
            printw("\n");
}

void print_paths_a_b(unsigned int a, unsigned int b) {
    clear();
    printw("Лучший(е) путь(и) из города %s в город %s:\n", cities[a].c_str(), cities[b].c_str());
    attron(COLOR_PAIR(4));
    printw("Всего найдено %llu путей.\n", paths_to.at(b).size());
    attroff(COLOR_PAIR(4));
    long long unsigned int i = 1;
        for (auto x: paths_to.at(b))
        {
            attron(COLOR_PAIR(3));
            printw("%d) ", i++);
            attron(COLOR_PAIR(3));
            path_print(x);
        }
    // printw("\n\nСкроллить окно с помощью стрелочек вверх/вниз. Нажмите любую клавишу, чтобы вернуться в главное меню...");
    printw("\n\nНажмите любую клавишу, чтобы вернуться в главное меню...");
    curs_set(0);
    // getch();
    // bool exit = false;
    // while (!exit) {
    //     switch (getch())
    //     {
    //     case KEY_UP:
    //         scrl(-1);
    //         break;
    //     case KEY_DOWN:
    //         scrl(1);
    //         break;
    //     default:
    //         exit = true;
    //         break;
    //     }
    // }

}
void print_paths_multiple(unsigned int a) {
    clear();
    printw("Все пути из города %s:\n", cities[a].c_str());
    unsigned int num_visited = 0;
    for (bool x: used)
        if (x) num_visited++;
    long long unsigned int i = 1;
        for (unsigned int j = 0; j < paths_to.size(); j++) {
            if (used[j])
            {
                attron(A_UNDERLINE);
                printw("Пути в город %s:", cities[j].c_str());
                attroff(A_UNDERLINE);
                printw(" ");
                for (auto x: paths_to.at(j))
                {
                    attron(COLOR_PAIR(3));
                    printw("%d) ", i++);
                    attron(COLOR_PAIR(3));
                    path_print(x);
                }
            }
        }
    attron(COLOR_PAIR(4));
    printw("Всего найдено %llu путей в %u городов.", i-1, num_visited);
    attroff(COLOR_PAIR(4));
    printw("\n\nНажмите любую клавишу, чтобы вернуться в главное меню...");
    curs_set(0);
    getch();
}

void scr_1() {
    unsigned int a = city_input();
    unsigned int b = city_input("в котором закончите движение:");
    transport_input();
    clear();
    printw("Расчитываем пути...");
    if (dijkstra(a, b, true, false)) // задание №1
        // не нашли путь из А в Б
        we_are_sorry();
    else 
        print_paths_a_b(a, b);
}
void scr_2() {
    unsigned int a = city_input();
    unsigned int b = city_input("в котором закончите движение:");
    transport_input();
    clear();
    printw("Расчитываем пути...");
    if (dijkstra(a, b, false, true)) // задание №2
        // не нашли путь из А в Б
        we_are_sorry();
    else
        print_paths_a_b(a, b);
}
void scr_3() {
    unsigned int a = city_input();
    unsigned int b = city_input("в котором закончите движение:");
    transport_input();
    clear();
    printw("Расчитываем пути...");
    if (dijkstra(a, b, true, true, true)) // задание №3
        // не нашли путь из А в Б
        we_are_sorry();
    else 
        print_paths_a_b(a, b);
}
void scr_4() {
    unsigned int a = city_input();
    long long unsigned int limit = limit_input();
    transport_input();
    clear();
    printw("Расчитываем пути...");
    dijkstra(a, a, false, true, false, true, limit); // задание №4
    print_paths_multiple(a);
}
void scr_5() {
    unsigned int a = city_input();
    long long unsigned int limit = limit_input("времени, limit_time:");
    transport_input();
    clear();
    printw("Расчитываем пути...");
    dijkstra(a, a, true, true, false, true, limit); // задание №5
    print_paths_multiple(a);
}


void select_screen() {

    const char *choices[] = {
        "1. Среди кратчайших по времени путей между двумя городами найти путь минимальной стоимости, если город достижим из города отправления.",
        "2. Среди путей между двумя городами найти путь минимальной стоимости, если город достижим из города отправления.",
        "3. Найти путь между 2-мя городами, минимальный по числу посещённых городов.",
        "4. Найти множество городов, достижимых из города отправления не более чем за limit_cost денег и вывести кратчайшие по деньгам пути к ним.",
        "5. Найти множество городов, достижимых из города отправления не более чем за limit_time времени и вывести кратчайшие по времени пути к ним.",
        "Выйти из программы"
    };
    unsigned int n_choices = sizeof(choices) / sizeof(char *);

    while (1) {
        curs_set(0);
        noecho();
        unsigned int highlight = 0;
        bool valid = false;
        while (!valid) {
            clear();
            printw("%s\n", "Возможные протоколы:");
            for (unsigned int i = 0; i < n_choices; i++)
            {
                if (i == highlight)
                {
                    addch('>');
                    // attron(A_BOLD);
                    attron(COLOR_PAIR(1));
                    // attron(A_UNDERLINE);
                    printw("%s\n", choices[i]);
                    // attroff(A_BOLD);
                    attroff(COLOR_PAIR(1));
                    // attroff(A_UNDERLINE);
                }
                else
                {
                    addch(' ');
                    printw("%s\n", choices[i]);
                }
            }
            switch (getch()) {
                case KEY_UP:
                    if (highlight > 0) highlight--;
                    break;
                case KEY_DOWN:
                    if (highlight < n_choices - 1) highlight++;
                    break;
                case (int)'\n':
                    valid = true;
                    break;
                default:
                    break;
            }
        }
        switch (highlight)
        {
        case 0: scr_1(); break;
        case 1: scr_2(); break;
        case 2: scr_3(); break;
        case 3: scr_4(); break;
        case 4: scr_5(); break;
        default:
            break;
        }
        if (highlight == 5) break;
    }
}


int main(int argc, char** argv) {
    setlocale(LC_ALL,"ru_RU.UTF-8");  
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    // std::cout << __cplusplus;

    read_data("data.txt");

    // быстрый запуск внутри командной строки (по кодам городов, без управления транспортом)
    if (argc > 1)
    {
        if ((atoi(argv[1]) == 1) && (argc == 5)){
            dijkstra(atoi(argv[2]), atoi(argv[3]), (bool)atoi(argv[4]), false, false, false, 0, true);
            return 0;
        }
        else if ((atoi(argv[1]) == 2) && (argc == 5)){
            dijkstra(atoi(argv[2]), atoi(argv[3]), (bool)atoi(argv[4]), true, false, false, 0, true);
            return 0;
        }
        else if ((atoi(argv[1]) == 3) && (argc == 4)){
            dijkstra(atoi(argv[2]), atoi(argv[3]), true, true, true, false, 0, true);
            return 0;
        }
        else if ((atoi(argv[1]) == 4) && (argc == 4)){
            dijkstra(atoi(argv[2]), atoi(argv[2]), false, true, false, true, atoi(argv[3]), true);
            return 0;
        }
        else if ((atoi(argv[1]) == 5) && (argc == 4)){
            dijkstra(atoi(argv[2]), atoi(argv[2]), true, true, false, true, atoi(argv[3]), true);
            return 0;
        }
    }
    // dijkstra(0, 3, true, false); // задание №1
    // dijkstra(0, 3, false, true); // задание №2
    // dijkstra(0, 3, true, true);
    // dijkstra(0, 3, false, false);
    // // dijkstra(0, 5, false, false);
    // dijkstra(0, 0, true, true);
    // dijkstra(0, 2, true, true, true); // задание №3
    // dijkstra(0, 0, false, true, false, true, 500); // задание №4
    // dijkstra(0, 0, true, true, false, true, 120); // задание №5

    // log();
    initscr(); 
    cbreak(); 
    noecho();
    scrollok(stdscr, 1); // enable window to be scrolled
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    // init_pair(2, 8, COLOR_WHITE); //gray-white
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    keypad(stdscr, true);

    select_screen();

    endwin();
    return 0;
}