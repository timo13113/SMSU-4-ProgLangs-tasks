

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

