#include "libs.hpp"
#include "classes.hpp"


/**
 * Модифицированный алгоритм Дийкстры (ищет один из оптимальных путей)
 * @param from номер города из которого идем
 * @param to номер города в который идем
 * @param graph граф
 * @param transport_whitelist список разрешенных/запрещенных видов транспорта
 * @return опциональная пара {маппинг путей, флаги посещенных городов}
 */
SinglePathMapping dijkstra_light(
    const unsigned int from,
    const unsigned int to,
    const Graph &graph,
    const std::vector<bool> &transport_whitelist,
    const long long unsigned int limit,
    const int instruction
) {
    SinglePathMapping paths = SinglePathMapping(graph.size(), from, instruction);
    
    std::function<bool(DoubleWeightedIndex)> halting;
    std::function<bool(unsigned int, Cruise, unsigned int)> is_better;
    
    switch (instruction)
    {
    case 1:
        halting = [&to, &limit] (DoubleWeightedIndex v) { return v.index == to; };
        is_better = [&paths] (unsigned int a, Cruise c, unsigned int b) 
        { 
            if (paths.get_path_to(a).time_cost + c.cruise_time == paths.get_path_to(b).time_cost)
                return paths.get_path_to(a).money_cost + c.cruise_fare < paths.get_path_to(b).money_cost;
            else
                return paths.get_path_to(a).time_cost + c.cruise_time < paths.get_path_to(b).time_cost;
        };

        break;
    case 2:
        halting = [&to, &limit] (DoubleWeightedIndex v) { return v.index == to; };
        is_better = [&paths] (unsigned int a, Cruise c, unsigned int b) 
        { 
            return paths.get_path_to(a).money_cost + c.cruise_fare < paths.get_path_to(b).money_cost;
        };
        break;
    case 3:
        halting = [&to, &limit] (DoubleWeightedIndex v) { return v.index == to; };
        is_better = [&paths] (unsigned int a, Cruise c, unsigned int b) 
        { 
            return paths.get_path_to(a).n_tickets + 1 < paths.get_path_to(b).n_tickets;
        };
        break;
    case 4:
        halting = [&to, &limit] (DoubleWeightedIndex v) { return v.weight1 > limit; };
        is_better = [&paths] (unsigned int a, Cruise c, unsigned int b) 
        { 
            return paths.get_path_to(a).money_cost + c.cruise_fare < paths.get_path_to(b).money_cost;
        };
        break;
    default: // 5
        halting = [&to, &limit] (DoubleWeightedIndex v) { return v.weight1 > limit; };
        is_better = [&paths] (unsigned int a, Cruise c, unsigned int b) 
        { 
            return paths.get_path_to(a).time_cost + c.cruise_time < paths.get_path_to(b).time_cost;
        };
        break;
    }

    while (1) {
        auto v = paths.pop();
        if (!v.has_value())
            // прошли все достижимые вершины
            break;
        if (halting(*v))
            // достигли цели
            break;
        unsigned int idx = v.value().index;

        // std::cout << "-- " << idx << " --\n";

        for (auto edges : graph[idx]) {
            for (auto cruise : edges.second) {
                if (transport_whitelist[cruise.transport_type_id])
                    if (!paths.has_path_to(edges.first) || is_better(idx, cruise, edges.first))
                    {
                        // std::cout << idx << " - " << edges.first << " - " << "" << "\n";
                        paths.push(paths.get_path_to(idx) + std::make_pair(cruise, edges.first));
                    }
            }
        }
    }
    return paths; // все хорошо и мы выходим из функции нормально
}
