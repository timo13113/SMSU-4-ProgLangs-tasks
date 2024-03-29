#pragma once

#include "libs.hpp"

/**
 * Класс маппинга строк и чисел
*/
class NamesMapping{
    std::unordered_map<std::string, unsigned int> str_to_idx;
    std::vector<std::string> idx_to_str;

public:
    NamesMapping() = default;
    
    auto _get_iter(std::string str) const {
        return str_to_idx.find(str);
    }

    /// @brief добавляет элемент в мапу и в вектор и возвращает его индекс
    unsigned int add_else_get(std::string str) {
        auto iter = this->_get_iter(str);
        // если не найден
        if (iter == str_to_idx.end()) {
            // std::cout << "not found!!!\n";
            // добавляем в оба хранилища
            unsigned int s = (unsigned int)idx_to_str.size();
            str_to_idx[str] = s;
            idx_to_str.push_back(str);
            return s;
        }
        else {
            // std::cout << "already exists!!!\n";
            return iter->second;
        }
    }

    /// @brief есть ли элемент в мапе
    bool has(std::string str) const {
        return str_to_idx.find(str) != str_to_idx.end();
    }

    /// @brief возвращает индекс по элементу
    auto get(std::string str) const {
        return str_to_idx.find(str)->second;
    }

    /// @brief возвращает количество элементов 
    auto size() const {
        if (idx_to_str.size() != str_to_idx.size())
            throw std::range_error("async size!!!");
        return idx_to_str.size(); 
    }

    /// @brief возвращает элемент по индексу 
    std::string operator[] (unsigned int i) const {
        return idx_to_str.at(i);
    }
};

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
    std::vector<std::vector<std::pair<unsigned int, std::set<Cruise>>>> graph;
public:
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


class DoubleWeightedIndex{
public:
    uint64_t weight1, weight2;
    unsigned int index;
    DoubleWeightedIndex() = default;
    DoubleWeightedIndex(uint64_t _weight1, uint64_t _weight2, unsigned int _index) {
        weight1 = _weight1;
        weight2 = _weight2;
        index = _index;
    }
};

class comparator_double {
public:
    bool operator() (DoubleWeightedIndex const &l, DoubleWeightedIndex const &r) {
        if (l.weight1 == r.weight1)
            return l.weight2 > r.weight2;
        else
            return l.weight1 > r.weight1;
    }
};


/**
 * Класс маппинга пути из некой точки выхода - источника в каждую другую точку, 
 * если соответствующий путь существует/был найден
 */
class SinglePathMapping {
    unsigned int source_id;
    unsigned int n;
    int instruction;
    std::unordered_map<unsigned int, std::optional<Path>> paths;
    std::vector<unsigned int> used;
    std::vector<DoubleWeightedIndex> distances;


public:
    /**
     *  @brief задание маппинга путей
     *  @param _n количество точек
     *  @param _source_id айди источника
     *  @param _instruction режим работы
     */
    SinglePathMapping(unsigned int _n, unsigned int _source_id, int _instruction) {
        // тривиальный путь из себя в себя который не требует транспорта и имеет (0, 0) стоимость
        paths[_source_id] = Path(std::vector<Cruise>(), _source_id, std::vector<unsigned int>());
        source_id = _source_id;
        n = _n;
        instruction = _instruction;
        distances.push_back(DoubleWeightedIndex(0, 0, source_id));
        std::make_heap(distances.begin(), distances.end(), comparator_double());
    }
    // // кол-во прямых рейсов пути из источника в вершину
    // unsigned int len_to(unsigned int i) const { return paths.at(i).value().n_tickets; }
    // bool len_to_is_infty(unsigned int i) const { 
    //     return paths.at(i).value().n_tickets == UINT32_MAX; 
    // }
    // // длительность пути из источника в вершину
    // uint64_t time_to(unsigned int i) const { return paths.at(i).value().time_cost; }
    // bool time_to_is_infty(unsigned int i) const { 
    //     return paths.at(i).value().time_cost == UINT64_MAX; 
    // }
    // // стоимость пути из источника в вершину
    // uint64_t fare_to(unsigned int i) const { return paths.at(i).value().money_cost; }
    // bool fare_to_is_infty(unsigned int i) const { 
    //     return paths.at(i).value().money_cost == UINT64_MAX; 
    // }

    void push(Path path) {
        unsigned int destination = path.transfer_cities.back();
        DoubleWeightedIndex item;
        switch (instruction)
        {
        case 1:
            item = DoubleWeightedIndex(path.time_cost, path.money_cost, destination);
            break;
        case 2:
            item = DoubleWeightedIndex(path.money_cost, 0, destination);
            break;
        case 3:
            item = DoubleWeightedIndex(path.n_tickets, 0, destination);
            break;
        case 4:
            item = DoubleWeightedIndex(path.money_cost, 0, destination);
            break;
        default: // 5
            item = DoubleWeightedIndex(path.time_cost, 0, destination);
            break;
        }
        distances.push_back(item);
        std::push_heap(distances.begin(), distances.end(), comparator_double());
        paths[destination] = path;
    }

    std::optional<DoubleWeightedIndex> pop() {
        if (distances.empty())
            return {};
        std::pop_heap(distances.begin(), distances.end(), comparator_double());
        DoubleWeightedIndex ret = distances.back();
        distances.pop_back();
        used.push_back(ret.index);
        if (!distances.empty()) 
        {
            std::pop_heap(distances.begin(), distances.end(), comparator_double());
            while (ret.index == distances.back().index) {
                std::pop_heap(distances.begin(), distances.end(), comparator_double());
                distances.pop_back();
            }
        }
        return ret;
    }

    // есть ли в маппинге путь из источника в аргумент
    bool has_path_to(unsigned int i) const { 
        if (paths.find(i) == paths.end())
            return false;
        return paths.at(i).has_value(); 
    }
    // возвращает путь из источника в вершину
    auto get_path_to(unsigned int i) const { return paths.at(i).value(); }
    // задает путь из источника в вершину
    // void set_path_to(unsigned int i, Path p) { paths.at(i) = p; }

    unsigned int get_source_id() const { return source_id; }
    unsigned int get_n() const { return n; }
    auto get_used() const { return used; }

};

