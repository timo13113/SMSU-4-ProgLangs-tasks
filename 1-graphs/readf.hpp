#include <utility>
#include <iostream>
#include <string>
#include <vector>

#include "classes.hpp"

unsigned int find_else_add(std::vector<std::string> &v, std::string tofind) {
    // попробовать найти в векторе городов новый
    auto from_index = std::find(v.begin(), v.end(), tofind); 
    if (from_index == v.end()) { 
        // если его там нет, то добавить к концу вектора
        v.push_back(tofind);
        return v.size() - 1;
    } else {
        // если есть, то вернуть его индекс в векторе
        return std::distance(v.begin(), from_index);
    }
}

void read_data(
    std::string filename, 
    std::vector<std::string> &city_names,
    std::vector<std::string> &transport_names, 
    Graph &graph
    ) {
    // read data from file
    std::ifstream data(filename);
    std::string delimiter = "\" \"";
    
    // for each line in file
    for (std::string line; std::getline(data, line);) 
    {
        // в строке есть символ комментария
        if (line.find('#') != std::string::npos)
            line.erase(line.find('#'), line.size() - line.find('#'));
        if (line.size() == 0) 
            // строка пустая
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
        unsigned int cruise_time = std::stoi(remaining.substr(0, remaining.find(' '))); 
        remaining.erase(0, remaining.find(' ') + 1);
        unsigned int cruise_fare = std::stoi(remaining);

        unsigned int id_from = find_else_add(city_names, from_city);
        unsigned int id_to = find_else_add(city_names, to_city);
        unsigned int id_transport = find_else_add(transport_names, transport_type);

        if (graph.size() < city_names.size()) {
            graph.resize(city_names.size());
        }
        graph.addNodeToGraph(id_from, id_to, Cruise(id_transport, cruise_time, cruise_fare));
    }
}
