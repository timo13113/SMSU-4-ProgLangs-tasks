#include <iostream>
#include <fstream>
#include <string>
#include <vector>
// #include <array>
#include <windows.h>
#include <utility>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <optional>

#include "classes.hpp"

// Для рёбер графа (рейсов) требуется создать собственный класс, где вместо имён городов и видов транспорта
// должны хранится их идентификаторы. считается, что городов и видов транспорта не может быть больше, чем
// 2^32. Пути реализовать отдельным классом и определить для них operator+, добавляющий новый элемент к пути,
// и operator[] для доступа к элементу пути. При выполнении задания предолагается по максимому использовать
// контейнеры STL.

//TODO:
// - граф. оболочка и интерфейс


std::set<std::pair<unsigned int /* из */, std::pair<unsigned int /* в */, Cruise/* ребро */>>> all_cruises;

std::vector<std::vector<std::pair<unsigned int /* направление */, std::set<Cruise/* набор ребер из А в Б */>>>> graph;

std::vector<std::string> cities;
std::vector<std::string> transport_types;

std::vector<bool> transport_whitelist; // разрешенный транспорт

void dijkstra(
        unsigned int from, // город из которого идем
        unsigned int to, // город в который идем
        bool time_over_money = true, // считаем ли мы приорететнее время или деньги
        bool keep_unefficient_paths = false, // учитывать ли пути, оптимальные по главной переменной, но не оптимальные по второстепенной переменной
        bool count_only_number_of_cities = false, // для задания №3, когда нужно посчитать минимальное количество пересадок
        bool have_limit = false, // если у нас есть предел на количество денег/времени, которое мы можем использовать
        long long unsigned int limit = 0
    ) {
    std::cout << "dijkstra started from " << from << " to " << to << "\n";
    unsigned int n = cities.size();
    std::vector<std::vector<Path>> paths_to; // кратчайшие пути из from в город по индексу
    paths_to.resize(n);
    paths_to.at(from) = std::vector<Path>(1, Path(std::vector<Cruise>(), from, std::vector<unsigned int>())); // тривиальный путь из себя в себя который не требует транспорта и имеет (0, 0) стоимость
    std::vector<bool> used; // прошли этот город или нет
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
                    if (!used.at(j) && (!v.has_value() || paths_to.at(j).at(0).transfer_cities.size() < paths_to.at(*v).at(0).transfer_cities.size())) // найти неисследованную вершину с минимальными затратами до нее
                        v = j;
            if (!v.has_value() || paths_to.at(*v).at(0).time_cost == UINT64_MAX) // если мы исследовали все вершины но не дошли куда надо ИЛИ мы вынуждены начинать с вершины в которую мы никогда не заходили
            ///////////////////////////////////////////////
            // TODO - нормально обработать ситуацию выше //
            ///////////////////////////////////////////////
            {
                std::cout << "!!! no path from A to B !!!\n";
                return; // то мы не можем прийти к этой вершине вовсе
            }
            if (have_limit && paths_to.at(*v).at(0).transfer_cities.size() > limit) // если мы считаем предел и он превышен
            {
                halt_on_limit = true;
                break;
            }
            
            used[*v] = true; // исследовать ее
            for (auto edges: graph.at(*v)) // для каждого набора круизов из нее
            {
                for (auto cruise: edges.second) // для каждого круиза в город под номером edges.first
                {
                    if ((paths_to.at(edges.first).empty() || paths_to.at(*v).at(0).transfer_cities.size() + 1 < paths_to.at(edges.first).at(0).transfer_cities.size()) && (transport_whitelist[cruise.transport_type_id])) // если мы можем прийти туда быстрее и по разрешенному транспорту
                    {
                        std::vector<Path> newpaths; // все новые пути через v в edges.first
                        for (auto x: paths_to.at(*v)) {
                            newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                        }
                        paths_to.at(edges.first) = newpaths; // вставить новые пути в paths_to
                    }
                    else if ((paths_to.at(*v).at(0).transfer_cities.size() + 1 == paths_to.at(edges.first).at(0).transfer_cities.size()) && (transport_whitelist[cruise.transport_type_id])) // если мы можем прийти туда так же быстро и по разрешенному транспорту
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
                std::cout << "!!! no path from A to B !!!\n";
                return; // то мы не можем прийти к этой вершине вовсе
            }
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
                    if ((paths_to.at(edges.first).empty() || paths_to.at(*v).at(0).time_cost + cruise.cruise_time < paths_to.at(edges.first).at(0).time_cost) && (transport_whitelist[cruise.transport_type_id])) // если мы можем прийти туда быстрее и по разрешенному транспорту
                    {
                        std::vector<Path> newpaths; // все новые пути через v в edges.first
                        for (auto x: paths_to.at(*v)) {
                            newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                        }
                        paths_to.at(edges.first) = newpaths; // вставить новые пути в paths_to
                    }
                    else if ((paths_to.at(*v).at(0).time_cost + cruise.cruise_time == paths_to.at(edges.first).at(0).time_cost) && (transport_whitelist[cruise.transport_type_id])) // если мы можем прийти туда так же быстро и по разрешенному транспорту
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
                std::cout << "!!! no path from A to B !!!\n";
                return; // то мы не можем прийти к этой вершине вовсе
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
                    if ((paths_to.at(edges.first).empty() || paths_to.at(*v).at(0).money_cost + cruise.cruise_fare < paths_to.at(edges.first).at(0).money_cost) && (transport_whitelist[cruise.transport_type_id])) // если мы можем прийти туда быстрее и по разрешенному транспорту
                    {
                        std::vector<Path> newpaths; // все новые пути через v в edges.first
                        for (auto x: paths_to.at(*v)) {
                            newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                        }
                        paths_to.at(edges.first) = newpaths; // вставить новые пути в paths_to
                    }
                    else if ((paths_to.at(*v).at(0).money_cost + cruise.cruise_fare == paths_to.at(edges.first).at(0).money_cost) && (transport_whitelist[cruise.transport_type_id])) // если мы можем прийти туда так же быстро и по разрешенному транспорту
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
    if (halt_on_limit) {
        std::cout << "\n";
        for (unsigned int i = 0; i < n; i++) {
            if (used[i])
            {
                std::cout << cities[i] <<":\n";
                for (auto x: paths_to.at(i))
                {
                    x.print();
                }
            }
        }
    }
    else {
        std::cout << cities[to] << "\n";
        if (count_only_number_of_cities)
            std::cout << "answer - " << paths_to.at(to).at(0).transfer_cities.size() << " cruises minimum\n";
        for (auto x: paths_to.at(to))
        {
            x.print();
        }
    }
    
    std::cout << "\n";
    
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


int main() {
    setlocale(LC_ALL,"");  
    // std::cout << __cplusplus;

    read_data("data.txt");

    log();
    dijkstra(0, 3, true, false); // задание №1
    dijkstra(0, 3, false, true); // задание №2
    dijkstra(0, 3, true, true);
    dijkstra(0, 3, false, false);
    // dijkstra(0, 5, false, false);
    dijkstra(0, 0, true, true);
    dijkstra(0, 2, true, true, true); // задание №3
    dijkstra(0, 0, false, true, false, true, 500); // задание №4
    dijkstra(0, 0, true, true, false, true, 120); // задание №5

    // std::cout << cities.size();
    
    return 0;
}