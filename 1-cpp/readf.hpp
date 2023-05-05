#include <utility>
#include <iostream>
#include <string>
#include <vector>

#include "classes.hpp"

void read_data(
    std::string filename, 
    NamesMapping &city_names,
    NamesMapping &transport_names, 
    Graph &graph,
    std::fstream &log
    ) {
    // read data from file
    std::ifstream data(filename);
    std::string delimiter = "\" \"";
    
    std::string from_city, to_city, remaining, transport_type;
    unsigned int cruise_time, cruise_fare, id_from, id_to, id_transport;
    
    // for each line in file
    uint64_t counter = 0;
    for (std::string line; std::getline(data, line);) 
    {
        // if (counter % 100000 == 0) 
        //     std::cout << counter << "\n";
        // в строке есть символ комментария
        if (line.find('#') != std::string::npos)
            line.erase(line.find('#'), line.size() - line.find('#'));
        if (line.size() == 0) 
            // строка пустая
            continue;
        
        
        ++counter;
        from_city = line.substr(0, line.find(delimiter));
        from_city.erase(0, 1); // remove heading '"'
        line.erase(0, line.find(delimiter) + delimiter.length());
        to_city = line.substr(0, line.find(delimiter));
        line.erase(0, line.find(delimiter) + delimiter.length());
        remaining = line.substr(0, line.find(delimiter));
        transport_type = remaining.substr(0, remaining.find(' '));
        transport_type.erase(transport_type.length()-1, 1); // remove trailing '"'
        remaining.erase(0, remaining.find(' ') + 1);
        cruise_time = std::stoi(remaining.substr(0, remaining.find(' '))); 
        remaining.erase(0, remaining.find(' ') + 1);
        cruise_fare = std::stoi(remaining);

        id_from = city_names.add_else_get(from_city);
        id_to = city_names.add_else_get(to_city);
        id_transport = transport_names.add_else_get(transport_type);
        // std::cout << "here!!!!!!!\n";
        if (graph.size() < city_names.size()) {
            graph.resize(city_names.size());
        }

        // std::cout << city_names.size() << " " << id_from << " " << id_to << " " << id_transport << " " << "\n";
        graph.add_node_to_graph(id_from, id_to, Cruise(id_transport, cruise_time, cruise_fare));
        // std::cout << "here!!!!!!!\n";

    }
    log << "read from file, total lines processed - " << counter << "\n";
}
