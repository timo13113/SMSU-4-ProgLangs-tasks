#include "libs.hpp"
#include "classes.hpp"
#include "dijkstra.hpp"
#include "debug.hpp"

unsigned int city_input(
    const std::vector<std::string> &city_names,
    const char* display_text = "из которого начнете движение:"
    ) {
    bool valid = false;
    bool warn = false;
    unsigned int city_id;
    std::string bad_city;
    echo();
    curs_set(1);
    while (!valid) {
        clear();
        printw("%s%s\n\n", "Введите город, ", display_text);
        if (warn) {
            attron(COLOR_PAIR(2));
            printw("%s - такого города нет в базе!\n", bad_city.c_str());
            attroff(COLOR_PAIR(2));
        }
        char input[200];
        getnstr(input, 200);
        // std::cout << err;
        std::string city(input);
        // std::cout << city << "\n";
        auto city_index = std::find(city_names.begin(), city_names.end(), city);
        if (city_index == city_names.end()) {
            warn = true;
            bad_city = city;
        }
        else {
            city_id = std::distance(city_names.begin(), city_index);
            valid = true;
        }
    }
    return city_id;
}
long long unsigned int limit_input(
    const char* display_text = "деньгам, limit_cost:"
    ) {
    bool valid = false;
    bool warn = false;
    long long unsigned int limit;
    std::string bad_limit;
    echo();
    curs_set(1);
    while (!valid) {
        clear();
        printw("%s%s\n\n", "Введите ваш предел по ", display_text);
        if (warn) {
            attron(COLOR_PAIR(2));
            printw("Введите валидное число!\n");
            attroff(COLOR_PAIR(2));
        }
        char input[200];
        getnstr(input, 200);
        char* endptr;
        limit = strtoull(input, &endptr, 10);
        if (input == endptr)
            warn = true;
        else
            valid = true;
    }
    return limit;
}
void transport_input(
    const std::vector<std::string> &transport_names, 
    std::vector<bool> &transport_whitelist
    ) {
    std::fill(transport_whitelist.begin(), transport_whitelist.end(), true);
    std::vector<std::string> bad_transports;
    std::vector<std::pair<bool, std::string>> good_transports;
    echo(); 
    curs_set(1);
    while (1) {
        clear();
        printw("%s\n\n", "Введите транспорт, по которому вы не будете перемещаться, через запятую c пробелом.\nКогда закончите вводить транспорт, оставьте пустую строку и нажмите Enter.");
        if (good_transports.size()) {
            attron(COLOR_PAIR(3));
            printw("Обработан транспорт: ");
            for (auto x: good_transports) {
                if (x.first) printw("записан %s ", x.second.c_str());
                else printw("удален %s ", x.second.c_str());
            }
            printw("\n");
            attroff(COLOR_PAIR(3));
        }
        good_transports.clear();

        if (bad_transports.size()) {
            attron(COLOR_PAIR(2));
            printw("Не обработан транспорт: ");
            for (auto x: bad_transports) {
                printw("%s ", x.c_str());
            }
            printw("\n");
            attroff(COLOR_PAIR(2));
        }
        bad_transports.clear();
        
        char input[200];
        getnstr(input, 200);
        
        long long unsigned int pos = 0;
        std::string delimiter = ", ";
        std::string input_transports(input);
        if (input_transports.size() == 0) 
            return;
        
        bool was_found = false;
        while ((pos = input_transports.find(delimiter)) != std::string::npos)
        {
            std::string suggested_transport = input_transports.substr(0, pos);
            // find suggested_transport in actual transports
            was_found = false;
            for (unsigned int i = 0; i < transport_names.size(); i++)
            {
                if (transport_names.at(i) == suggested_transport) {
                    transport_whitelist.at(i) = transport_whitelist.at(i) xor true;
                    was_found = true;
                    good_transports.push_back(
                        std::make_pair(transport_whitelist.at(i), suggested_transport));
                    break;
                }
            }
            if (!was_found)
                bad_transports.push_back(suggested_transport);
            
            input_transports.erase(0, pos + delimiter.size());
        }
        // find suggested_transport in actual transports
        was_found = false;
        for (unsigned int i = 0; i < transport_names.size(); i++)
        {
            if (transport_names.at(i) == input_transports) {
                transport_whitelist.at(i) = transport_whitelist.at(i) xor true;
                was_found = true;
                good_transports.push_back(
                    std::make_pair(transport_whitelist.at(i), input_transports));
                break;
            }
        }
        if (!was_found)
            bad_transports.push_back(input_transports);
        
        // for (bool x: transport_whitelist)
        //     std::cout << x;
        // std::cout << "\n";
    }
}

void we_are_sorry(std::fstream &log) {
    clear();
    std::string truly_sorry1 = 
    std::string("Мы не нашли путь между этими городами с такими условиями :(\n\n");
    std::string truly_sorry2 = 
    std::string("  _   __  ___ _  ___ __   ___ _   _ _   _  ___  \n") + 
    std::string(" | | /  |/ __| |/ / '_ \\ / _ \\ |_| | |_| |/ _ \\ \n") + 
    std::string(" | |'/| | (__|   <| |_) |  __/  _  |  _  |  __/ \n") + 
    std::string(" |__/ |_|\\___|_|\\_\\ .__/ \\___|_| |_|_| |_|\\___| \n") + 
    std::string("                  |_|                           \n") +
    std::string("  ___ ___ __  _  ____ _   __   ___  ___  ___ ___   \n") +
    std::string(" / __/ _ \\\\ \\| |/ / _` | /  \\ / _ \\/ _ \\/   V   \\  \n") + 
    std::string("| (_| (_) |>     < (_| |/ /\\ \\  __/  __/  /\\ /\\  \\ \n") +
    std::string(" \\___\\___//_/|_|\\_\\__,_/_/  \\_\\___|\\___|_/  V  \\__\\\n");
    attron(COLOR_PAIR(2));
    printw(truly_sorry1.c_str());
    printw(truly_sorry2.c_str());
    attroff(COLOR_PAIR(2));
    printw("\n\nНажмите любую клавишу, чтобы вернуться в главное меню...");
    curs_set(0);
    log << "Путь не был найден\n";
    getch();
}
void path_print(
    Path path, 
    const std::vector<std::string> &city_names, 
    const std::vector<std::string> &transport_names,
    std::fstream &log
    ) {
    printw("Путь: %s", city_names[path.start_city].c_str());
    log << "Путь: \n\t" << city_names[path.start_city];
    for (unsigned int i = 0; i < path.length(); i++)
    {
        std::string cruise_str;
        cruise_str = 
            // "(взяв билет на " + transport_names.at(path[i].second.transport_type_id) + 
            // ", проведя в пути " + std::to_string(path[i].second.cruise_time) + 
            // " минут, потратив "+ std::to_string(path[i].second.cruise_fare) + " рублей)";
            "(" + transport_names.at(path[i].second.transport_type_id) + 
            ", " + std::to_string(path[i].second.cruise_time) + 
            ", "+ std::to_string(path[i].second.cruise_fare) + ")";
        printw(" -> %s -> %s", 
            cruise_str.c_str(), 
            city_names[path[i].first].c_str()
        );
        log << " -> " << cruise_str.c_str() << " -> \n\t" << city_names[path[i].first].c_str();
    }
    attron(COLOR_PAIR(3));
    printw("\nВсего будет потрачено %llu минут, %llu рублей и будет осуществлено %llu прямых рейсов.", 
        path.time_cost, path.money_cost, path.length());
    attroff(COLOR_PAIR(3));
    log << "\nВсего будет потрачено " << path.time_cost 
        << " минут, " << path.money_cost 
        << " рублей и будет осуществлено " << path.length() << " прямых рейсов.\n";
    printw("\n");
}

void print_paths_a_b(
    const unsigned int a, 
    const unsigned int b, 
    const std::vector<std::string> &city_names, 
    const PathsMapping &found_paths,
    const std::vector<std::string> &transport_names,
    std::fstream &log
    ) {
    clear();
    printw("Лучший(е) путь(и) из города %s в город %s:\n", 
        city_names[a].c_str(), city_names[b].c_str());
    log << "Лучший(е) путь(и) из города " << city_names[a] 
        << " в город " << city_names[b] << ":\n";
    attron(COLOR_PAIR(4));
    printw("Всего найдено %llu путей.\n", found_paths.get_num_of_paths_to(b));
    log << "Всего найдено " << found_paths.get_num_of_paths_to(b) << " путей.\n";
    attroff(COLOR_PAIR(4));
    long long unsigned int i = 1;
        for (auto x: found_paths.get_paths_to(b))
        {
            log << i << ") ";
            attron(COLOR_PAIR(3));
            printw("%d) ", i++);
            attron(COLOR_PAIR(3));
            path_print(x, city_names, transport_names, log);
        }
    // printw("\n\nСкроллить окно с помощью стрелочек вверх/вниз. Нажмите любую клавишу, чтобы вернуться в главное меню...");
    printw("\n\nНажмите любую клавишу, чтобы вернуться в главное меню...");
    curs_set(0);
    getch();
    // bool exit = false;
    // while (!exit) {
    //     switch (getch())
    //     {
    //     case KEY_UP:
    //         scrl(-1);
    //         break;
    //     case KEY_DOWN:
    //         scrl(1);
    //         break;
    //     default:
    //         exit = true;
    //         break;
    //     }
    // }

}
void print_paths_multiple(
    const unsigned int a, 
    const std::vector<std::string> &city_names, 
    const std::vector<bool> &city_was_reached,
    const PathsMapping &found_paths, 
    const std::vector<std::string> &transport_names,
    std::fstream &log
    ) {
    clear();
    printw("Все пути из города %s:\n", city_names[a].c_str());
    log << "Все пути из города " << city_names[a] << ":\n";
    unsigned int num_visited = 0;
    for (bool x: city_was_reached)
        if (x) num_visited++;
    long long unsigned int i = 1;
        for (unsigned int j = 0; j < found_paths.n; j++) {
            if ((found_paths.source_id == j) && !SHOW_TRIVIAL_PATHS)
                continue;
            if (city_was_reached.at(j))
            {
                attron(A_UNDERLINE);
                printw("Пути в город %s:", city_names[j].c_str());
                attroff(A_UNDERLINE);
                printw(" ");
                log << "Пути в город " << city_names[j] << ": ";
                for (auto x: found_paths.get_paths_to(j))
                {
                    log << i << ") ";
                    attron(COLOR_PAIR(3));
                    printw("%d) ", i++);
                    attron(COLOR_PAIR(3));
                    path_print(x, city_names, transport_names, log);
                }
            }
        }
    attron(COLOR_PAIR(4));
    printw("Всего найдено %llu путей в %u городов.", i-1, num_visited-(int)!SHOW_TRIVIAL_PATHS);
    log << "Всего найдено " << i-1 << " путей в " << num_visited << " городов.";
    attroff(COLOR_PAIR(4));
    printw("\n\nНажмите любую клавишу, чтобы вернуться в главное меню...");
    log << "\n";
    curs_set(0);
    getch();
}

void scr_1(
    const std::vector<std::string> &city_names,
    const std::vector<std::string> &transport_names, 
    const Graph &graph,
    std::fstream &log
    ) {
    unsigned int a = city_input(city_names);
    unsigned int b = city_input(city_names, "в котором закончите движение:");
    std::vector<bool> transport_whitelist;
    transport_whitelist.resize(transport_names.size(), true);
    transport_input(transport_names, transport_whitelist);
    clear();
    printw("Расчитываем пути...");
    auto t_dijkstra_heavy = timeit_not_void(dijkstra_heavy, "dijkstra_heavy", log);
    auto ans = t_dijkstra_heavy(
        a, b, graph, transport_whitelist, true, false, false, false, 0, false);
    log << "------\n### Результат выполнения задания #1 ###\n------\n";
    if (!ans.has_value()) // задание №1
        // не нашли путь из А в Б
        we_are_sorry(log);
    else 
        print_paths_a_b(a, b, city_names, ans.value().first, transport_names, log);
    log << "------\n#######################################\n------\n";
}
void scr_2(
    const std::vector<std::string> &city_names,
    const std::vector<std::string> &transport_names, 
    const Graph &graph,
    std::fstream &log
    ) {
    unsigned int a = city_input(city_names);
    unsigned int b = city_input(city_names, "в котором закончите движение:");
    std::vector<bool> transport_whitelist;
    transport_whitelist.resize(transport_names.size(), true);
    transport_input(transport_names, transport_whitelist);
    clear();
    printw("Расчитываем пути...");
    auto t_dijkstra_heavy = timeit_not_void(dijkstra_heavy, "dijkstra_heavy", log);
    auto ans = t_dijkstra_heavy(
        a, b, graph, transport_whitelist, false, true, false, false, 0, false);
    log << "------\n### Результат выполнения задания #2 ###\n------\n";
    if (!ans.has_value()) // задание №2
        // не нашли путь из А в Б
        we_are_sorry(log);
    else
        print_paths_a_b(a, b, city_names, ans.value().first, transport_names, log);
    log << "------\n#######################################\n------\n";
}
void scr_3(
    const std::vector<std::string> &city_names,
    const std::vector<std::string> &transport_names, 
    const Graph &graph,
    std::fstream &log
    ) {
    unsigned int a = city_input(city_names);
    unsigned int b = city_input(city_names, "в котором закончите движение:");
    std::vector<bool> transport_whitelist;
    transport_whitelist.resize(transport_names.size(), true);
    transport_input(transport_names, transport_whitelist);
    clear();
    printw("Расчитываем пути...");
    auto t_dijkstra_heavy = timeit_not_void(dijkstra_heavy, "dijkstra_heavy", log);
    auto ans = t_dijkstra_heavy(
        a, b, graph, transport_whitelist, true, true, true, false, 0, false);
    log << "------\n### Результат выполнения задания #3 ###\n------\n";
    if (!ans.has_value()) // задание №3
        // не нашли путь из А в Б
        we_are_sorry(log);
    else 
        print_paths_a_b(a, b, city_names, ans.value().first, transport_names, log);
    log << "------\n#######################################\n------\n";
}
void scr_4(
    const std::vector<std::string> &city_names,
    const std::vector<std::string> &transport_names, 
    const Graph &graph,
    std::fstream &log
    ) {
    unsigned int a = city_input(city_names);
    long long unsigned int limit = limit_input();
    std::vector<bool> transport_whitelist;
    transport_whitelist.resize(transport_names.size(), true);
    transport_input(transport_names, transport_whitelist);
    clear();
    printw("Расчитываем пути...");
    auto t_dijkstra_heavy = timeit_not_void(dijkstra_heavy, "dijkstra_heavy", log);
    auto ans = t_dijkstra_heavy(
        a, a, graph, transport_whitelist, false, true, false, true, limit, false);
    log << "------\n### Результат выполнения задания #4 ###\n------\n";
    print_paths_multiple(
        a, city_names, ans.value().second, ans.value().first, transport_names, log);
    log << "------\n#######################################\n------\n";
}
void scr_5(
    const std::vector<std::string> &city_names,
    const std::vector<std::string> &transport_names, 
    const Graph &graph,
    std::fstream &log
    ) {
    unsigned int a = city_input(city_names);
    long long unsigned int limit = limit_input("времени, limit_time:");
    std::vector<bool> transport_whitelist;
    transport_whitelist.resize(transport_names.size(), true);
    transport_input(transport_names, transport_whitelist);
    clear();
    printw("Расчитываем пути...");
    auto t_dijkstra_heavy = timeit_not_void(dijkstra_heavy, "dijkstra_heavy", log);
    auto ans = t_dijkstra_heavy(
        a, a, graph, transport_whitelist, true, true, false, true, limit, false);
    log << "------\n### Результат выполнения задания #5 ###\n------\n";
    print_paths_multiple(
        a, city_names, ans.value().second, ans.value().first, transport_names, log);
    log << "------\n#######################################\n------\n";
}

void select_screen(
    const std::vector<std::string> &city_names,
    const std::vector<std::string> &transport_names, 
    const Graph &graph,
    std::fstream &log
    ) {

    const char *choices[] = {
        "1. Среди кратчайших по времени путей между двумя городами найти путь минимальной стоимости, если город достижим из города отправления.",
        "2. Среди путей между двумя городами найти путь минимальной стоимости, если город достижим из города отправления.",
        "3. Найти путь между 2-мя городами, минимальный по числу посещённых городов.",
        "4. Найти множество городов, достижимых из города отправления не более чем за limit_cost денег и вывести кратчайшие по деньгам пути к ним.",
        "5. Найти множество городов, достижимых из города отправления не более чем за limit_time времени и вывести кратчайшие по времени пути к ним.",
        "Выйти из программы"
    };
    unsigned int n_choices = sizeof(choices) / sizeof(char *);

    while (1) {
        curs_set(0);
        noecho();
        unsigned int highlight = 0;
        bool valid = false;
        while (!valid) {
            clear();
            printw("%s\n", "Возможные протоколы:");
            for (unsigned int i = 0; i < n_choices; i++)
            {
                if (i == highlight)
                {
                    addch('>');
                    // attron(A_BOLD);
                    attron(COLOR_PAIR(1));
                    // attron(A_UNDERLINE);
                    printw("%s\n", choices[i]);
                    // attroff(A_BOLD);
                    attroff(COLOR_PAIR(1));
                    // attroff(A_UNDERLINE);
                }
                else
                {
                    addch(' ');
                    printw("%s\n", choices[i]);
                }
            }
            switch (getch()) {
                case KEY_UP:
                    if (highlight > 0) highlight--;
                    break;
                case KEY_DOWN:
                    if (highlight < n_choices - 1) highlight++;
                    break;
                case (int)'\n':
                    valid = true;
                    break;
                default:
                    break;
            }
        }
        switch (highlight)
        {
        case 0: scr_1(city_names, transport_names, graph, log); break;
        case 1: scr_2(city_names, transport_names, graph, log); break;
        case 2: scr_3(city_names, transport_names, graph, log); break;
        case 3: scr_4(city_names, transport_names, graph, log); break;
        case 4: scr_5(city_names, transport_names, graph, log); break;
        default:
            break;
        }
        if (highlight == 5) break;
    }
}

void init_tui(
    const std::vector<std::string> &city_names,
    const std::vector<std::string> &transport_names, 
    const Graph &graph,
    std::fstream &log
    ) {
    
    initscr(); 
    cbreak(); // config input
    noecho();
    scrollok(stdscr, 1); // enable window to be scrolled
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    // init_pair(2, 8, COLOR_WHITE); //gray-white
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    keypad(stdscr, true);

    select_screen(city_names, transport_names, graph, log);

    endwin();
}