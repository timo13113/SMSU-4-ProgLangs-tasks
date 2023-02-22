#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <utility>
#include <iostream>

class Cruise{
    public:
        unsigned int transport_type_id;
        unsigned int cruise_time = 0;
        unsigned int cruise_fare = 0;
        Cruise() = default;
        Cruise(
            unsigned int tt, 
            unsigned int ct, 
            unsigned int cf) {
            transport_type_id = tt;
            cruise_time = ct;
            cruise_fare = cf;
            // std::cout << "Cruise object created\n";
        };
        void print() {
            std::cout << "{" << transport_type_id << ", " << cruise_time << ", " << cruise_fare << "}\n";
        };
        bool operator > (Cruise const & r) const {
            if (this->transport_type_id != r.transport_type_id) {return this->transport_type_id > r.transport_type_id;}
            else if (this->cruise_time != r.cruise_time) {return this->cruise_time > r.cruise_time;}
            else if (this->cruise_fare != r.cruise_fare) {return this->cruise_fare > r.cruise_fare;}
            else return false;
        }
        bool operator == (Cruise const & r) const {
            return 
            (this->transport_type_id == r.transport_type_id)
                && (this->cruise_time == r.cruise_time) 
                && (this->cruise_fare == r.cruise_fare);
        }
        bool operator < (Cruise const & r) const { 
            if (this->transport_type_id != r.transport_type_id) {return this->transport_type_id < r.transport_type_id;}
            else if (this->cruise_time != r.cruise_time) {return this->cruise_time < r.cruise_time;}
            else if (this->cruise_fare != r.cruise_fare) {return this->cruise_fare < r.cruise_fare;}
            else return false;
        }
};

std::ostream& operator <<(std::ostream & os, const Cruise & cruise) {
    os << "{" << cruise.transport_type_id << ", t=" << cruise.cruise_time << ", $=" << cruise.cruise_fare << "}";
    return os;
}

class Path{
    public:
        unsigned int start_city;
        std::vector<unsigned int> transfer_cities;
        std::vector<Cruise> cruises;
        long long unsigned int time_cost, money_cost = UINT64_MAX;

        Path() = default; // дефолтный конструктор оставляет стоимости равными максинт для адекватной работы Дейкстры
        Path(std::vector<Cruise> crs, unsigned int start, std::vector<unsigned int> cities) { // конструктор с пустым вектором делает стоимости 0, конструктор с непустым вектором работает как ожидается
            cruises = crs; 
            long long unsigned int t, c = 0;
            for (auto x: crs)
            {
                t += x.cruise_time;
                c += x.cruise_fare;
            }
            time_cost = t;
            money_cost = c;
            start_city = start;
            transfer_cities = cities;
        };
        void print() {
            std::cout << "\tPath: " << start_city;
            for (unsigned int i = 0; i < cruises.size(); i++)
            {
                std::cout << " -> " << cruises.at(i) << " -> " << transfer_cities.at(i);
            }
            std::cout << "\n";
        }
        Path operator + (std::pair<Cruise, unsigned int> in) { // оператор добавления отправления в конец пути
            auto new_cruises = this->cruises;
            new_cruises.push_back(in.first);
            auto new_cities = this->transfer_cities;
            new_cities.push_back(in.second);
            Path newpath(new_cruises, this->start_city, new_cities);
            return newpath;
        };
        Cruise operator [] (unsigned int i) { // получить i-й элемент пути
            return this->cruises.at(i);
        };
};
