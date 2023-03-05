#include <utility>
#include <iostream>
#include <string>
#include <vector>

#include "classes.hpp"

void read_data(
    std::string filename, 
    std::vector<std::string> &city_names,
    std::vector<std::string> &transport_names, 
    std::vector<bool> &transport_whitelist,
    std::vector<std::vector<std::pair<unsigned int /* направление */, std::set<Cruise/* набор ребер из А в Б */>>>> &graph
    ) {
    std::ifstream data(filename); // read data from file
    std::string delimiter = "\" \"";
    std::set<std::pair<unsigned int /* из */, std::pair<unsigned int /* в */, Cruise/* ребро */>>> all_cruises;

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

        auto from_index = std::find(city_names.begin(), city_names.end(), from_city); // попробовать найти в векторе городов новый
        if (from_index == city_names.end()) { // если его там нет, то добавить к концу вектора
            city_names.push_back(from_city);
            id_from = city_names.size() - 1;
        } else {// если есть, то вернуть его индекс в векторе
            id_from = std::distance(city_names.begin(), from_index);
        }
        auto to_index = std::find(city_names.begin(), city_names.end(), to_city);
        if (to_index == city_names.end()) {// если его там нет, то добавить к концу вектора
            city_names.push_back(to_city);
            id_to = city_names.size() - 1;
        } else { // если есть, то вернуть его индекс в векторе
            id_to = std::distance(city_names.begin(), to_index);
        }
        auto transport_index = std::find(transport_names.begin(), transport_names.end(), transport_type); // попробовать найти в векторе транспортов новый
        if (transport_index == transport_names.end()) {// если его там нет, то добавить к концу вектора
            transport_names.push_back(transport_type);
            id_transport = transport_names.size() - 1;
        } else { // если есть, то вернуть его индекс в векторе
            id_transport = std::distance(transport_names.begin(), transport_index);
        }

        all_cruises.insert({id_from, {id_to, Cruise(id_transport, cruise_time, cruise_fare)}}); // вставить новый путь в общий граф
    }

    graph.resize(city_names.size()); // подогнать граф по размеру чтобы вмещать все города
    transport_whitelist.resize(transport_names.size(), true); // подогнать белый спискок по размеру чтобы вмещать все виды транспорта

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
