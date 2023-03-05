#include <iostream>
#include <utility>

// #include "dijkstra.hpp"

// std::set<std::pair<unsigned int /* из */, std::pair<unsigned int /* в */, Cruise/* ребро */>>> all_cruises;
// std::vector<std::vector<std::pair<unsigned int /* направление */, std::set<Cruise/* набор ребер из А в Б */>>>> graph;
// std::vector<std::string> cities;
// std::vector<std::string> transport_types;
// std::vector<bool> transport_whitelist; // разрешенный транспорт
// std::vector<std::vector<Path>> paths_to; // кратчайшие пути из from в город по индексу
// std::vector<bool> used; // прошли этот город или нет





// void debug_mode(int argc, char** argv) {
//     if ((atoi(argv[1]) == 1) && (argc == 5)){
//         dijkstra_heavy(atoi(argv[2]), atoi(argv[3]), (bool)atoi(argv[4]), false, false, false, 0, true);
//     }
//     else if ((atoi(argv[1]) == 2) && (argc == 5)){
//         dijkstra_heavy(atoi(argv[2]), atoi(argv[3]), (bool)atoi(argv[4]), true, false, false, 0, true);
//     }
//     else if ((atoi(argv[1]) == 3) && (argc == 4)){
//         dijkstra_heavy(atoi(argv[2]), atoi(argv[3]), true, true, true, false, 0, true);
//     }
//     else if ((atoi(argv[1]) == 4) && (argc == 4)){
//         dijkstra_heavy(atoi(argv[2]), atoi(argv[2]), false, true, false, true, atoi(argv[3]), true);
//     }
//     else if ((atoi(argv[1]) == 5) && (argc == 4)){
//         dijkstra_heavy(atoi(argv[2]), atoi(argv[2]), true, true, false, true, atoi(argv[3]), true);
//     }
// }

// void log() {
//     std::cout << "Cities: ";
//     for (auto x: cities){
//         std::cout << x << " ";
//     }
//     std::cout << "\nTransports: ";
//     for (auto x: transport_types){
//         std::cout << x << " ";
//     }
//     std::cout << "\nGraph:\n";
//     int i = 0;
//     for (auto c1: graph)
//     {
//         std::cout << "from " << cities[i++] << "\n";
//         for (auto c2: c1)
//         {
//             std::cout << "\tto " << cities[c2.first] << "\n";
//             for (auto crs: c2.second)
//             {
//                 std::cout << "\t\tpath " << transport_types[crs.transport_type_id] << ", " << crs.cruise_time <<  ", " << crs.cruise_fare << "\n";
//             }
//             std::cout << "\t total paths: " << c2.second.size() << "\n";
//         }
//     }
// }

void measure_time () { return; }

void measure_memory () { return; }

