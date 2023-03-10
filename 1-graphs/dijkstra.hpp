#include "libs.hpp"
#include "classes.hpp"


/**
 * Модифицированный алгоритм Дийкстры (ищет все оптимальные пути)
 * @param from номер города из которого идем
 * @param to номер города в который идем
 * @param graph граф
 * @param transport_whitelist список разрешенных/запрещенных видов транспорта
 * @param time_over_money считаем ли мы приорететнее время поверх денег
 * @param keep_unefficient_paths учитывать ли пути, оптимальные по главной переменной, но не оптимальные по второстепенной переменной
 * @param count_only_number_of_city_names для задания №3, когда нужно посчитать минимальное количество пересадок
 * @param have_limit если у нас есть предел на количество денег/времени, которое мы можем использовать
 * @return опциональная пара {маппинг путей, флаги посещенных городов}
 */
std::optional<std::pair<PathsMapping, std::vector<bool>>> dijkstra_heavy(
        const unsigned int from,
        const unsigned int to,
        const Graph &graph,
        const std::vector<bool> &transport_whitelist,
        const bool time_over_money = true,
        const bool keep_unefficient_paths = false,
        const bool count_only_number_of_city_names = false,
        const bool have_limit = false,
        const long long unsigned int limit = 0,
        const bool logs = false
    ) {
    if (logs) 
        std::cout << "dijkstra started from " << from << " to " << to << "\n";
    unsigned int n = graph.size();
    PathsMapping paths(n, from);
    std::vector<bool> used; // прошли этот город или нет
    used.resize(n);
    bool halt_on_limit = false;
    bool halt_condition;
    if (have_limit)
        halt_condition = !halt_on_limit;
    else 
        halt_condition = !used[to];

    // если мы минимизируем количество пересадок
    if (count_only_number_of_city_names) { 
        // пока не дошли куда надо или пока не превысили лимит
        while (halt_condition) 
        {
            // начальная вершина
            std::optional<unsigned int> v;
            for (unsigned int j = 0; j < n; j++)
                // если есть пути в вершину
                if (paths.hasPathsTo(j))
                    // найти неисследованную вершину с минимальными затратами до нее
                    if (!used.at(j) && (!v.has_value() || paths.lenTo(j) < paths.lenTo(*v))) 
                        v = j;
            // если мы дошли до всех вершин до которых можем дойти но так и не привысили лимит 
            // (если мы вообще считаем лимит), т.е. вынуждены начинать с вершины с максинтом
            if (have_limit && (!v.has_value() || paths.lenToIsInfty(*v))) {
                halt_on_limit = true;
                break;
            }
            // если мы исследовали все вершины но не дошли куда надо 
            // ИЛИ мы вынуждены начинать с вершины в которую мы никогда не заходили
            if (!v.has_value() || paths.lenToIsInfty(*v)) {
                // std::cout << "!!! no path from A to B !!!\n";
                return {}; // то мы не можем прийти к этой вершине вовсе
            }
            // если мы считаем предел и он превышен
            if (have_limit && paths.lenTo(*v) > limit) 
            {
                halt_on_limit = true;
                break;
            }

            // исследовать ее
            used[*v] = true;

            // для каждого набора круизов из нее
            for (auto edges: graph[*v]) {
                // для каждого круиза в город под номером edges.first
                for (auto cruise: edges.second) {
                    // если транспорт круиза разрешен
                    if (transport_whitelist.at(cruise.transport_type_id)) {
                        // если мы можем прийти туда быстрее
                        if (!paths.hasPathsTo(edges.first) || paths.lenTo(*v) + 1 < paths.lenTo(edges.first)) {
                            // все новые пути через v в edges.first
                            std::vector<Path> newpaths;
                            for (auto x: paths.getPathsTo(*v)) {
                                // дополнить к путям в v новый круиз до edges.first
                                newpaths.push_back(x + std::make_pair(cruise, edges.first)); 
                            }
                            // вставить новые пути в paths
                            paths.setPathsTo(edges.first, newpaths);
                        }
                        // если мы можем прийти туда так же быстро
                        else if (paths.lenTo(*v) + 1 == paths.lenTo(edges.first)) {
                            for (auto x: paths.getPathsTo(*v)) {
                                // дополнить к путям в v новый круиз до edges.first
                                paths.addPathTo(edges.first, x + std::make_pair(cruise, edges.first)); 
                            }
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
    // если мы ищем сначала самый быстрый путь, а среди них находим самый дешевый
    else if (time_over_money) { 
        // пока не дошли куда надо или пока не превысили лимит
        while (halt_condition)
        {
            // начальная вершина
            std::optional<unsigned int> v; 
            for (unsigned int j = 0; j < n; j++)
                if (paths.hasPathsTo(j)) // если есть пути в вершину
                    // найти неисследованную вершину с минимальными затратами до нее
                    if (!used.at(j) && (!v.has_value() || paths.timeTo(j) < paths.timeTo(*v))) 
                        v = j;
            // если мы дошли до всех вершин до которых можем дойти но так и не привысили лимит 
            // (если мы вообще считаем лимит), т.е. вынуждены начинать с вершины с максинтом
            if (have_limit && (!v.has_value() || paths.timeToIsInfty(*v))) 
            {
                halt_on_limit = true;
                break;
            }
            // если мы исследовали все вершины но не дошли куда надо 
            // ИЛИ мы вынуждены начинать с вершины в которую мы никогда не заходили
            if (!v.has_value() || paths.timeToIsInfty(*v))
            {
                // std::cout << "!!! no path from A to B !!!\n";
                return {}; // то мы не можем прийти к этой вершине вовсе
            }
            // если мы считаем предел и он превышен
            if (have_limit && paths.timeTo(*v) > limit)
            {
                halt_on_limit = true;
                break;
            }

            // исследовать ее
            used[*v] = true;

            // в исследованную вершину есть набор путей (минимум 1 путь), 
            // оптимальных по времени как минимум, по времени и деньгам как максимум. 
            // если их несколько, то их затраты одинаковы по главной или по обеим переменным

            // для каждого набора круизов из нее 
            for (auto edges: graph[*v]) 
            {
                // для каждого круиза в город под номером edges.first
                for (auto cruise: edges.second) 
                {
                    // если транспорт круиза разрешен
                    if (transport_whitelist.at(cruise.transport_type_id)) {
                        // если мы можем прийти туда быстрее
                        if (!paths.hasPathsTo(edges.first) || paths.timeTo(*v) + cruise.cruise_time < paths.timeTo(edges.first)) {
                            // все новые пути через v в edges.first
                            std::vector<Path> newpaths;
                            for (auto x: paths.getPathsTo(*v)) {
                                // дополнить к путям в v новый круиз до edges.first
                                newpaths.push_back(x + std::make_pair(cruise, edges.first));
                            }
                            // вставить новые пути в paths
                            paths.setPathsTo(edges.first, newpaths);
                        } 
                        // если мы можем прийти туда так же быстро
                        else if (paths.timeTo(*v) + cruise.cruise_time == paths.timeTo(edges.first)) {
                            // если можем прийти так же дешево (или если нам все равно насколько дешево)
                            if (keep_unefficient_paths || paths.fareTo(*v) + cruise.cruise_fare == paths.fareTo(edges.first)) 
                            {
                                for (auto x: paths.getPathsTo(*v)) {
                                    // дополнить к путям в v новый круиз до edges.first
                                    paths.addPathTo(edges.first, x + std::make_pair(cruise, edges.first)); 
                                }
                            }
                            // если мы можем прийти дешевле
                            else if (paths.fareTo(*v) + cruise.cruise_fare < paths.fareTo(edges.first)) 
                            {
                                std::vector<Path> newpaths; // все новые пути через v в edges.first
                                for (auto x: paths.getPathsTo(*v)) {
                                    // дополнить к путям в v новый круиз до edges.first
                                    newpaths.push_back(x + std::make_pair(cruise, edges.first)); 
                                }
                                // вставить новые пути в paths
                                paths.setPathsTo(edges.first, newpaths);
                            }
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
    // если мы ищем сначала самый дешевый путь, а среди них находим самый быстрый
    else {
        // пока не дошли куда надо или пока не превысили лимит
        while (halt_condition)
        {
            // начальная вершина
            std::optional<unsigned int> v;
            for (unsigned int j = 0; j < n; j++)
                // если есть пути в вершину
                if (paths.hasPathsTo(j)) 
                    // найти неисследованную вершину с минимальными затратами до нее
                    if (!used.at(j) && (!v.has_value() || paths.fareTo(j) < paths.fareTo(*v))) 
                        v = j;
            // если мы дошли до всех вершин до которых можем дойти но так и не привысили лимит 
            // (если мы вообще считаем лимит), т.е. вынуждены начинать с вершины с максинтом
            if (have_limit && (!v.has_value() || paths.fareToIsInfty(*v)))
            {
                halt_on_limit = true;
                break;
            }
            // если мы исследовали все вершины но не дошли куда надо 
            // ИЛИ мы вынуждены начинать с вершины в которую мы никогда не заходили
            if (!v.has_value() || paths.fareToIsInfty(*v))
            {
                // std::cout << "!!! no path from A to B !!!\n";
                return {}; // то мы не можем прийти к этой вершине вовсе
            }
            // если мы считаем предел и он превышен
            if (have_limit && paths.fareTo(*v) > limit)
            {
                halt_on_limit = true;
                break;
            }
            
            // исследовать ее
            used[*v] = true; 

            // в исследованную вершину есть набор путей (минимум 1 путь),
            // оптимальных по времени как минимум, по времени и деньгам как максимум. 
            // если их несколько, то их затраты одинаковы по главной или по обеим переменным
            
            // для каждого набора круизов из нее
            for (auto edges: graph[*v])
            {
                // для каждого круиза в город под номером edges.first
                for (auto cruise: edges.second) 
                {
                    // если транспорт круиза разрешен
                    if (transport_whitelist.at(cruise.transport_type_id)) {
                        // если мы можем прийти туда быстрее
                        if (!paths.hasPathsTo(edges.first) || paths.fareTo(*v) + cruise.cruise_fare < paths.fareTo(edges.first)) 
                        {
                            // все новые пути через v в edges.first
                            std::vector<Path> newpaths; 
                            for (auto x: paths.getPathsTo(*v)) {
                                // дополнить к путям в v новый круиз до edges.first
                                newpaths.push_back(x + std::make_pair(cruise, edges.first)); 
                            }
                            // вставить новые пути в paths
                            paths.setPathsTo(edges.first, newpaths);
                        }
                        // если мы можем прийти туда так же быстро
                        else if (paths.fareTo(*v) + cruise.cruise_fare == paths.fareTo(edges.first)) 
                        {
                            // если можем прийти так же дешево (или если нам все равно насколько дешево)
                            if (keep_unefficient_paths || paths.timeTo(*v) + cruise.cruise_time == paths.timeTo(edges.first)) 
                            {
                                for (auto x: paths.getPathsTo(*v)) {
                                    // дополнить к путям в v новый круиз до edges.first
                                    paths.addPathTo(edges.first, x + std::make_pair(cruise, edges.first)); 
                                }
                            }
                            // если мы можем прийти дешевле
                            else if (paths.timeTo(*v) + cruise.cruise_time < paths.timeTo(edges.first)) 
                            {
                                // все новые пути через v в edges.first
                                std::vector<Path> newpaths; 
                                for (auto x: paths.getPathsTo(*v)) {
                                    // дополнить к путям в v новый круиз до edges.first
                                    newpaths.push_back(x + std::make_pair(cruise, edges.first)); 
                                }
                                // вставить новые пути в paths
                                paths.setPathsTo(edges.first, newpaths);
                            }
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
                    for (auto x: paths.getPathsTo(i))
                    {
                        x.print();
                    }
                }
            }
        }
        else { // если мы считаем конкретные пути из А в Б (№1, 2, 3)
            std::cout << to << "\n";
            if (count_only_number_of_city_names)
                std::cout << "answer - " << paths.lenTo(to) << " cruises minimum\n";
            for (auto x: paths.getPathsTo(to))
            {
                x.print();
            }
        }
        std::cout << "\n";
    }
    return std::make_pair(paths, used); // все хорошо и мы выходим из функции нормально
}

// возвращает только один из оптимальных путей
// bool dijkstra_light () { return false; }
