#include "libs.hpp"
#include "classes.hpp"

// unsigned int from - номер города из которого идем
// unsigned int to - номер города в который идем
// std::vector<std::vector<std::pair<unsigned int /* направление */, std::set<Cruise/* набор ребер из А в Б */>>>> graph - граф
// std::vector<bool> &transport_whitelist - список разрешенных/запрещенных видов транспорта
// bool time_over_money - считаем ли мы приорететнее время поверх денег
// bool keep_unefficient_paths - учитывать ли пути, оптимальные по главной переменной, но не оптимальные по второстепенной переменной
// bool count_only_number_of_city_names - для задания №3, когда нужно посчитать минимальное количество пересадок
// bool have_limit - если у нас есть предел на количество денег/времени, которое мы можем использовать
std::optional<std::pair<
    std::vector<std::vector<Path>>, std::vector<bool>
    >> dijkstra_heavy(
        const unsigned int from,
        const unsigned int to,
        const std::vector<std::vector<std::pair<unsigned int /* направление */, std::set<Cruise/* набор ребер из А в Б */>>>> &graph,
        const std::vector<bool> &transport_whitelist,
        const bool time_over_money = true,
        const bool keep_unefficient_paths = false,
        const bool count_only_number_of_city_names = false,
        const bool have_limit = false,
        const long long unsigned int limit = 0,
        const bool logs = false
    ) {
    if (logs) 
        // std::cout << "dijkstra started from " << city_names[from] << " to " << city_names[to] << "\n";
        std::cout << "dijkstra started from " << from << " to " << to << "\n";
    unsigned int n = graph.size();
    std::vector<std::vector<Path>> found_paths; // кратчайшие пути из from в город по индексу
    found_paths.clear();
    found_paths.resize(n);
    found_paths.at(from) = std::vector<Path>(1, Path(std::vector<Cruise>(), from, std::vector<unsigned int>())); // тривиальный путь из себя в себя который не требует транспорта и имеет (0, 0) стоимость
    std::vector<bool> used; // прошли этот город или нет
    used.clear();
    used.resize(n);
    bool halt_on_limit = false;
    bool halt_condition;
    if (have_limit)
        halt_condition = !halt_on_limit;
    else 
        halt_condition = !used[to];
    if (count_only_number_of_city_names) { // если мы минимизируем количество пересадок
        while (halt_condition) // пока не дошли куда надо или пока не превысили лимит
        {
            std::optional<unsigned int> v; // начальная вершина
            for (unsigned int j = 0; j < n; j++)
                if (!found_paths.at(j).empty()) // если есть пути в вершину
                    if (!used.at(j) && (!v.has_value() || found_paths.at(j).at(0).length() < found_paths.at(*v).at(0).length())) // найти неисследованную вершину с минимальными затратами до нее
                        v = j;
            if (!v.has_value() || found_paths.at(*v).at(0).time_cost == UINT64_MAX) // если мы исследовали все вершины но не дошли куда надо ИЛИ мы вынуждены начинать с вершины в которую мы никогда не заходили
            ///////////////////////////////////////////////
            // TODO - нормально обработать ситуацию выше //
            ///////////////////////////////////////////////
            {
                // std::cout << "!!! no path from A to B !!!\n";
                return {}; // то мы не можем прийти к этой вершине вовсе
            }
            if (have_limit && found_paths.at(*v).at(0).length() > limit) // если мы считаем предел и он превышен
            {
                halt_on_limit = true;
                break;
            }
            
            used[*v] = true; // исследовать ее
            for (auto edges: graph.at(*v)) // для каждого набора круизов из нее
            {
                for (auto cruise: edges.second) // для каждого круиза в город под номером edges.first
                {
                    if ((found_paths.at(edges.first).empty() || found_paths.at(*v).at(0).length() + 1 < found_paths.at(edges.first).at(0).length()) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда быстрее и по разрешенному транспорту
                    {
                        std::vector<Path> newpaths; // все новые пути через v в edges.first
                        for (auto x: found_paths.at(*v)) {
                            newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                        }
                        found_paths.at(edges.first) = newpaths; // вставить новые пути в found_paths
                    }
                    else if ((found_paths.at(edges.first).empty() || found_paths.at(*v).at(0).length() + 1 == found_paths.at(edges.first).at(0).length()) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда так же быстро и по разрешенному транспорту
                    {
                        for (auto x: found_paths.at(*v)) {
                            found_paths.at(edges.first).push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
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
                if (!found_paths.at(j).empty()) // если есть пути в вершину
                    if (!used.at(j) && (!v.has_value() || found_paths.at(j).at(0).time_cost < found_paths.at(*v).at(0).time_cost)) // найти неисследованную вершину с минимальными затратами до нее
                        v = j;
            if (!v.has_value() || found_paths.at(*v).at(0).time_cost == UINT64_MAX) // если мы исследовали все вершины но не дошли куда надо ИЛИ мы вынуждены начинать с вершины в которую мы никогда не заходили
            {
                // std::cout << "!!! no path from A to B !!!\n";
                return {}; // то мы не можем прийти к этой вершине вовсе
            }
            // std::cout << v.value();
            if (have_limit && found_paths.at(*v).at(0).time_cost > limit) // если мы считаем предел и он превышен
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
                    if ((found_paths.at(edges.first).empty() || found_paths.at(*v).at(0).time_cost + cruise.cruise_time < found_paths.at(edges.first).at(0).time_cost) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда быстрее и по разрешенному транспорту
                    {
                        std::vector<Path> newpaths; // все новые пути через v в edges.first
                        for (auto x: found_paths.at(*v)) {
                            newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                        }
                        found_paths.at(edges.first) = newpaths; // вставить новые пути в found_paths
                    }
                    else if ((found_paths.at(edges.first).empty() || found_paths.at(*v).at(0).time_cost + cruise.cruise_time == found_paths.at(edges.first).at(0).time_cost) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда так же быстро и по разрешенному транспорту
                    {
                        if (keep_unefficient_paths || found_paths.at(*v).at(0).money_cost + cruise.cruise_fare == found_paths.at(edges.first).at(0).money_cost) // если можем прийти так же дешево (или если нам все равно насколько дешево)
                        {
                            for (auto x: found_paths.at(*v)) {
                                found_paths.at(edges.first).push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                            }
                        }
                        else if (found_paths.at(*v).at(0).money_cost + cruise.cruise_fare < found_paths.at(edges.first).at(0).money_cost) // если мы можем прийти дешевле
                        {
                            std::vector<Path> newpaths; // все новые пути через v в edges.first
                            for (auto x: found_paths.at(*v)) {
                                newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                            }
                            found_paths.at(edges.first) = newpaths; // вставить новые пути в found_paths
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
                if (!found_paths.at(j).empty()) // если есть пути в вершину
                    if (!used.at(j) && (!v.has_value() || found_paths.at(j).at(0).money_cost < found_paths.at(*v).at(0).money_cost)) // найти неисследованную вершину с минимальными затратами до нее
                        v = j;
            if (!v.has_value() || found_paths.at(*v).at(0).money_cost == UINT64_MAX) // если мы исследовали все вершины но не дошли куда надо ИЛИ мы вынуждены начинать с вершины в которую мы никогда не заходили
            {
                // std::cout << "!!! no path from A to B !!!\n";
                return {}; // то мы не можем прийти к этой вершине вовсе
            }
            if (have_limit && found_paths.at(*v).at(0).money_cost > limit) // если мы считаем предел и он превышен
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
                    if ((found_paths.at(edges.first).empty() || found_paths.at(*v).at(0).money_cost + cruise.cruise_fare < found_paths.at(edges.first).at(0).money_cost) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда быстрее и по разрешенному транспорту
                    {
                        std::vector<Path> newpaths; // все новые пути через v в edges.first
                        for (auto x: found_paths.at(*v)) {
                            newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                        }
                        found_paths.at(edges.first) = newpaths; // вставить новые пути в found_paths
                    }
                    else if ((found_paths.at(edges.first).empty() || found_paths.at(*v).at(0).money_cost + cruise.cruise_fare == found_paths.at(edges.first).at(0).money_cost) && (transport_whitelist.at(cruise.transport_type_id))) // если мы можем прийти туда так же быстро и по разрешенному транспорту
                    {
                        if (keep_unefficient_paths || found_paths.at(*v).at(0).time_cost + cruise.cruise_time == found_paths.at(edges.first).at(0).time_cost) // если можем прийти так же дешево (или если нам все равно насколько дешево)
                        {
                            for (auto x: found_paths.at(*v)) {
                                found_paths.at(edges.first).push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                            }
                        }
                        else if (found_paths.at(*v).at(0).time_cost + cruise.cruise_time < found_paths.at(edges.first).at(0).time_cost) // если мы можем прийти дешевле
                        {
                            std::vector<Path> newpaths; // все новые пути через v в edges.first
                            for (auto x: found_paths.at(*v)) {
                                newpaths.push_back(x + std::make_pair(cruise, edges.first)); // дополнить к путям в v новый круиз до edges.first
                            }
                            found_paths.at(edges.first) = newpaths; // вставить новые пути в found_paths
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
        if (count_only_number_of_city_names)
            std::cout << ", #";
        std::cout << "): ";
        if (halt_on_limit) { // если мы считаем все города в пределе, то мы выводим все, в которых были, пока предел не превысился (№4, 5)
            std::cout << "\n";
            for (unsigned int i = 0; i < n; i++) {
                if (used[i])
                {
                    std::cout << i << ":\n";
                    for (auto x: found_paths.at(i))
                    {
                        x.print();
                    }
                }
            }
        }
        else { // если мы считаем конкретные пути из А в Б (№1, 2, 3)
            std::cout << to << "\n";
            if (count_only_number_of_city_names)
                std::cout << "answer - " << found_paths.at(to).at(0).length() << " cruises minimum\n";
            for (auto x: found_paths.at(to))
            {
                x.print();
            }
        }
        std::cout << "\n";
    }
    return std::make_pair(found_paths, used); // все хорошо и мы выходим из функции нормально
}

// возвращает только один из оптимальных путей
bool dijkstra_light () { return false; }
