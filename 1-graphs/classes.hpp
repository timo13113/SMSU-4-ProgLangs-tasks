#pragma once

#include "libs.hpp"

/**
 * Класс ребра графа
 * 
 * @param transport_type_id код транспорта, по которому происходит перемещение
 * @param cruise_time затраченное время на переход
 * @param cruise_fare затраченные деньги на переход
 */
class Cruise {
public:
    unsigned int transport_type_id;
    unsigned int cruise_time = 0;
    unsigned int cruise_fare = 0;
    Cruise() = default;
    Cruise(unsigned int tt, unsigned int ct, unsigned int cf) {
        transport_type_id = tt;
        cruise_time = ct;
        cruise_fare = cf;
    }
    /// @brief вывод наполнения в консоль
    void print() {
        std::cout << "{" << transport_type_id
                    << ", " << cruise_time
                    << ", " << cruise_fare << "}\n";
    }
    bool operator > (Cruise const & r) const {
        if (this->transport_type_id != r.transport_type_id)
            return this->transport_type_id > r.transport_type_id;
        else if (this->cruise_time != r.cruise_time)
            return this->cruise_time > r.cruise_time;
        else if (this->cruise_fare != r.cruise_fare)
            return this->cruise_fare > r.cruise_fare;
        else
            return false;
    }
    bool operator == (Cruise const & r) const {
        return
        (this->transport_type_id == r.transport_type_id)
            && (this->cruise_time == r.cruise_time)
            && (this->cruise_fare == r.cruise_fare);
    }
    bool operator < (Cruise const & r) const {
        if (this->transport_type_id != r.transport_type_id)
            return this->transport_type_id < r.transport_type_id;
        else if (this->cruise_time != r.cruise_time)
            return this->cruise_time < r.cruise_time;
        else if (this->cruise_fare != r.cruise_fare)
            return this->cruise_fare < r.cruise_fare;
        else
            return false;
    }
};

std::ostream& operator <<(std::ostream & os, const Cruise & cruise) {
    os << "{" << cruise.transport_type_id
       << ", t=" << cruise.cruise_time
       << ", $=" << cruise.cruise_fare << "}";
    return os;
}

/**
 * Класс пути по графу
 * 
 * @param start_city город отправления
 * @param transfer_cities города на пути, в т.ч. город прибытия
 * @param cruises ребра перехода между соседними городами в пути
 * @param time_cost стоимость обхода по времени
 * @param money_cost стоимость обхода по деньгам
 * @param n_tickets стоимость обхода по пересадкам
 */
class Path {
public:
    unsigned int start_city;
    std::vector<unsigned int> transfer_cities;
    std::vector<Cruise> cruises;
    uint64_t time_cost, money_cost = UINT64_MAX;
    unsigned int n_tickets = UINT32_MAX;

    /// дефолтный конструктор оставляет стоимости равными максинт
    /// для адекватной работы Дейкстры
    Path() = default;

    /// конструктор с пустым вектором делает стоимости 0,
    /// конструктор с непустым вектором работает как ожидается
    Path(std::vector<Cruise> crs, unsigned int start, std::vector<unsigned int> cities) {
        cruises = crs;
        uint64_t t = 0, c = 0;
        unsigned int n = 0;
        for (auto x : crs) {
            t += x.cruise_time;
            c += x.cruise_fare;
            n++;
        }
        time_cost = t;
        money_cost = c;
        n_tickets = n;
        start_city = start;
        transfer_cities = cities;
    }

    /// @brief вывод наполнения в консоль
    void print() {
        std::cout << "\tPath: " << start_city;
        for (unsigned int i = 0; i < cruises.size(); i++) {
            std::cout << " -> " << cruises.at(i) << " -> " << transfer_cities.at(i);
        }
        std::cout << "\n";
    }
    /// оператор добавления отправления в конец пути
    Path operator + (std::pair<Cruise, unsigned int> in) {
        auto new_cruises = cruises;
        new_cruises.push_back(in.first);
        auto new_cities = transfer_cities;
        new_cities.push_back(in.second);
        Path newpath(new_cruises, start_city, new_cities);
        return newpath;
    }
    /// получить i-й элемент пути - пара {город, круиз}
    std::pair<unsigned int, Cruise> operator[] (unsigned int i) {
        return std::make_pair(transfer_cities.at(i), cruises.at(i));
    }
    unsigned int length() {
        if (transfer_cities.size() == cruises.size()) {
            return transfer_cities.size();
        } else {
            std::cout << "From Path class: bad contents size! something went terribly wrong";
            return 0;
        }
    }
};

/**
 * Класс графа
 */
class Graph {
public:
    std::vector<std::vector<std::pair<unsigned int, std::set<Cruise>>>> graph;
    Graph() = default;

    /**
     *  @brief Есть ли ребра между вершинами
     *  @param from откуда
     *  @param to куда
     *  @return итератор указывающий на ребра в Graph[from] идущие в to
     *  @return или пустой optional, если ребер нет
     */
    std::optional<std::vector<std::pair<unsigned int, std::set<Cruise>>>::iterator> 
    has_edges_between(unsigned int from, unsigned int to) {
        auto iter = std::find_if(
            graph.at(from).begin(), graph.at(from).end(), 
            [to](std::pair<unsigned int, std::set<Cruise>> x) -> bool {return x.first == to;});
        if (iter == graph.at(from).end()) {
            return {};
        } else {
            return std::make_optional(iter);
        }
    }
    void add_node_to_graph(unsigned int from, unsigned int to, Cruise edge) {
        auto edges = has_edges_between(from, to);
        
        if (!edges) {
            graph.at(from).push_back(std::make_pair(to, std::set<Cruise>{edge}));
        } else {
            graph.at(from).at(*edges - graph.at(from).begin()).second.insert(edge);
        }
    }
    void resize(unsigned int n) { graph.resize(n); }
    unsigned int size() const { return graph.size(); }
    auto operator[] (unsigned int i) const {
        return graph.at(i);
    }
};

/**
 * Класс маппинга путей из некой точки выхода - источника в каждую другую точку, 
 * если соответствующие пути существуют/были найдены
 */
class PathsMapping {
public:
    std::vector<std::vector<Path>> paths;
    unsigned int source_id;
    unsigned int n;
    /**
     *  @brief задание маппинга путей
     *  @param n количество точек
     *  @param _source_id айди источника
     */
    PathsMapping(unsigned int _n, unsigned int _source_id) {
        paths.resize(_n);
        // тривиальный путь из себя в себя который не требует транспорта и имеет (0, 0) стоимость
        paths.at(_source_id) = std::vector<Path>(
            1, Path(std::vector<Cruise>(), _source_id, std::vector<unsigned int>())
        );
        source_id = _source_id;
        n = _n;
    }
    // есть ли в маппинге путь из источника в аргумент
    bool has_paths_to(unsigned int dest) const { return !paths.at(dest).empty(); }
    // кол-во прямых рейсов первого из путей из источника в вершину
    unsigned int len_to(unsigned int i) const { return paths.at(i).at(0).n_tickets; }
    bool len_to_is_infty(unsigned int i) const { 
        return paths.at(i).at(0).n_tickets == UINT32_MAX; 
    }
    // длительность первого из путей из источника в вершину
    uint64_t time_to(unsigned int i) const { return paths.at(i).at(0).time_cost; }
    bool time_to_is_infty(unsigned int i) const { 
        return paths.at(i).at(0).time_cost == UINT64_MAX; 
    }
    // стоимость первого из путей из источника в вершину
    uint64_t fare_to(unsigned int i) const { return paths.at(i).at(0).money_cost; }
    bool fare_to_is_infty(unsigned int i) const { 
        return paths.at(i).at(0).money_cost == UINT64_MAX; 
    }

    // возвращает набор путей из источника в вершину
    auto get_paths_to(unsigned int i) const { return paths.at(i); }
    // возвращает количество путей из источника в вершину
    uint64_t get_num_of_paths_to(unsigned int i) const { return paths.at(i).size(); }
    // задает набор путей из источника в вершину
    void set_paths_to(unsigned int i, std::vector<Path> ps) { paths.at(i) = ps; }
    // добавляет путь в набор путей из источника в вершину
    void add_paths_to(unsigned int i, Path path) { paths.at(i).push_back(path); }
};

