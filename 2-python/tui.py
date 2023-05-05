import curses as c
from dijkstra import *


def init_tui(*args, **kwargs):
    c.wrapper(select_screen, *args, **kwargs)
    pass


def select_screen(
        stdscr: "c._CursesWindow",
        city_names: NamesMapping,
        transport_names: NamesMapping,
        graph, log):
    stdscr.scrollok(True)
    stdscr.clear()
    stdscr.refresh()

    c.start_color()
    c.init_pair(1, c.COLOR_BLACK, c.COLOR_WHITE)
    c.init_pair(2, c.COLOR_YELLOW, c.COLOR_BLACK)
    c.init_pair(3, c.COLOR_GREEN, c.COLOR_BLACK)
    c.init_pair(4, c.COLOR_CYAN, c.COLOR_BLACK)
    choices = [
        "1. Среди кратчайших по времени путей между двумя городами найти путь "
        "минимальной стоимости, если город достижим из города отправления.\n",
        "2. Среди путей между двумя городами найти путь минимальной стоимости, "
        "если город достижим из города отправления.\n",
        "3. Найти путь между 2-мя городами, минимальный по числу посещённых городов.\n",
        "4. Найти множество городов, достижимых из города отправления не более "
        "чем за limit_cost денег и вывести кратчайшие по деньгам пути к ним.\n",
        "5. Найти множество городов, достижимых из города отправления не более "
        "чем за limit_time времени и вывести кратчайшие по времени пути к ним.\n",
        "Выйти из программы\n"
    ]

    c.curs_set(0)

    while True:
        stdscr.clear()
        k = ' '
        highlight = 0
        valid = False
        while not valid:
            stdscr.clear()
            stdscr.addstr("Возможные протоколы:\n")
            for i, choice in enumerate(choices):
                if i == highlight:
                    stdscr.addch('>')
                    stdscr.addstr(choice, c.color_pair(1))
                else:
                    stdscr.addch(' ')
                    stdscr.addstr(choice)

            # stdscr.addstr(str(k))

            k = stdscr.getch()
            if k == c.KEY_UP:
                if highlight > 0:
                    highlight -= 1
            elif k == c.KEY_DOWN:
                if highlight < len(choices) - 1:
                    highlight += 1
            elif k == c.KEY_ENTER or k == 10:
                valid = True
            # else:
            #     pass
            stdscr.refresh()

        if highlight in [0, 1, 2, 3, 4]:
            manager_scr(highlight, stdscr, city_names, transport_names, graph, log)
        elif highlight == 5:
            break
        else:
            pass

    pass


def manager_scr(option,
                stdscr: 'c._CursesWindow',
                city_names: NamesMapping,
                transport_names: NamesMapping,
                graph, log):
    a = city_input(stdscr, city_names, "из которого начнете движение:")
    # print("going from:", a)
    b, limit = -1, -1
    if option + 1 in [1, 2, 3]:
        b = city_input(stdscr, city_names, "в котором закончите движение:")
        # print("going to:", b)
    elif option + 1 == 4:
        limit = limit_input(stdscr, "деньгам, limit_cost:")
        # print("limit:", b)
    elif option + 1 == 5:
        limit = limit_input(stdscr, "времени, limit_time:")
        # print("limit:", b)

    transport_whitelist: List[bool] = [True] * len(transport_names)
    transport_input(stdscr, transport_names, transport_whitelist)
    stdscr.clear()
    stdscr.addstr("Расчитываем пути...")
    stdscr.refresh()
    t_dijkstra = timeit_and_log("dijkstra", log)(dijkstra)

    # paths = dijkstra(a, b, limit, graph, transport_whitelist, len(city_names), option+1)
    paths, t = t_dijkstra(a, b, limit, graph, transport_whitelist, len(city_names), option+1)
    print(f"------\n### Результат выполнения задания #{option + 1} ###\n------", file=log)
    if option + 1 in [1, 2, 3]:
        if paths[b] is None:
            # не нашли путь из А в Б
            we_are_sorry(stdscr, log)
        else:
            print_paths_a_b(stdscr, a, b, city_names, paths[b], transport_names, log)
    else:
        print_paths_multiple(stdscr, a, city_names, paths, transport_names, log)
    print("------\n#######################################\n------", file=log)
    print(f"# python, t = {t} ms, "
          f"m = {get_max_memory_usage(None, True)} KB, "
          f"query = {option + 1} #", file=log)


def city_input(stdscr: 'c._CursesWindow',
               city_names: NamesMapping, display_text):
    valid = False
    warn = False
    city_id = -1
    bad_city = ""
    c.echo()
    c.curs_set(1)
    while not valid:
        stdscr.clear()
        stdscr.addstr("Введите город, " + display_text + "\n\n")
        if warn:
            # print(bad_city)
            stdscr.addstr(bad_city + " - такого города нет в базе!\n", c.color_pair(2))
        stdscr.refresh()
        city = stdscr.getstr(200).decode('UTF-8')
        if city not in city_names:
            warn = True
            bad_city = city
        else:
            city_id = city_names.get(city)
            valid = True
    return city_id


def transport_input(
        stdscr: 'c._CursesWindow',
        transport_names: NamesMapping,
        transport_whitelist: list[bool]):
    bad_transports: list[str] = []
    good_transports: list[Tuple[bool, str]] = []
    c.echo()
    c.curs_set(1)
    delimiter = ", "
    while True:
        stdscr.clear()
        stdscr.addstr("Введите транспорт, по которому вы не будете перемещаться, "
                      "через запятую c пробелом, введите его повторно чтобы "
                      "изменить настройку обратно.\nКогда закончите вводить "
                      "транспорт, оставьте пустую строку и нажмите Enter.\n")
        if good_transports:
            stdscr.addstr("Обработан транспорт: ", c.color_pair(3))
            for accept, transport in good_transports:
                if accept:
                    stdscr.addstr("записан " + transport + " ", c.color_pair(3))
                else:
                    stdscr.addstr("удален " + transport + " ", c.color_pair(3))
            stdscr.addstr("\n")
        good_transports.clear()

        if bad_transports:
            stdscr.addstr("Не обработан транспорт: " +
                          delimiter.join(bad_transports), c.color_pair(2))
            stdscr.addstr("\n")
        bad_transports.clear()

        input_transports: str = stdscr.getstr(200).decode('UTF-8')

        if not input_transports:
            return

        for suggested_transport in input_transports.split(delimiter):
            if transport_names.has(suggested_transport):
                i = transport_names.get(suggested_transport)
                transport_whitelist[i] = transport_whitelist[i] != True  # flip-flop
                good_transports.append((transport_whitelist[i], suggested_transport))
            else:
                bad_transports.append(suggested_transport)


def limit_input(stdscr: 'c._CursesWindow', display_text):
    valid = False
    warn = False
    limit = -1
    c.echo()
    c.curs_set(1)
    while not valid:
        stdscr.clear()
        stdscr.addstr("Введите ваш предел по " + display_text + "\n\n")
        if warn:
            stdscr.addstr("Введите валидное число!\n", c.color_pair(2))
        stdscr.refresh()
        limit = stdscr.getstr(200).decode('UTF-8')
        try:
            limit = int(limit)
            valid = True
        except ValueError:
            warn = True
    return limit


def print_paths_a_b(
        stdscr: 'c._CursesWindow', a, b,
        city_names: NamesMapping,
        path: Path,
        transport_names: NamesMapping, log):
    stdscr.clear()
    stdscr.addstr("Лучший путь из города " + city_names[a] +
                  " в город " + city_names[b] + ":\n")
    print("Лучший путь из города", city_names[a], "в город", city_names[b] + ":", file=log)
    path_print(stdscr, path, city_names, transport_names, log)
    stdscr.addstr("\n\nНажмите любую клавишу, чтобы вернуться в главное меню...")
    c.curs_set(0)
    stdscr.refresh()
    stdscr.getch()


def print_paths_multiple(
        stdscr: 'c._CursesWindow', a,
        city_names: NamesMapping,
        found_paths: SinglePathMapping,
        transport_names: NamesMapping, log):
    stdscr.clear()
    stdscr.addstr("Все пути из города " + city_names[a] + ":\n")
    print("Все пути из города " + city_names[a] + ":", file=log)
    num_visited = str(len(found_paths.used))
    for j in found_paths.used:
        # if found_paths.source_id == j:
        #     # && !SHOW_TRIVIAL_PATHS
        #     continue
        stdscr.addstr("Путь в город " + city_names[j] + ":", c.A_UNDERLINE)
        stdscr.addstr(" ")
        print("Путь в город " + city_names[j] + ": ", file=log)
        path_print(stdscr, found_paths[j], city_names, transport_names, log)

    stdscr.addstr("Всего найдено " + num_visited + " путей в " + num_visited + " городов.", c.color_pair(4))
    # printw("Всего найдено %llu путей в %u городов.", i-1, num_visited-(int)!SHOW_TRIVIAL_PATHS)
    print("Всего найдено " + num_visited + " путей в " + num_visited + " городов.", file=log)
    stdscr.addstr("\n\nНажмите любую клавишу, чтобы вернуться в главное меню...")
    c.curs_set(0)
    stdscr.refresh()
    stdscr.getch()


def we_are_sorry(stdscr: 'c._CursesWindow', log):
    stdscr.clear()
    truly_sorry1 = "Мы не нашли путь между этими городами с такими условиями :(\n\n"
    truly_sorry2 = "  _   __  ___ _  ___ __   ___ _   _ _   _  ___  \n" \
                   " | | /  |/ __| |/ / '_ \\ / _ \\ |_| | |_| |/ _ \\ \n" \
                   " | |'/| | (__|   <| |_) |  __/  _  |  _  |  __/ \n" \
                   " |__/ |_|\\___|_|\\_\\ .__/ \\___|_| |_|_| |_|\\___| \n" \
                   "                  |_|                           \n" \
                   "  ___ ___ __  _  ____ _   __   ___  ___  ___ ___   \n" \
                   " / __/ _ \\\\ \\| |/ / _` | /  \\ / _ \\/ _ \\/   V   \\  \n" \
                   "| (_| (_) |>     < (_| |/ /\\ \\  __/  __/  /\\ /\\  \\ \n" \
                   " \\___\\___//_/|_|\\_\\__,_/_/  \\_\\___|\\___|_/  V  \\__\\\n"
    stdscr.addstr(truly_sorry1, c.color_pair(2))
    stdscr.addstr(truly_sorry2, c.color_pair(2))
    stdscr.addstr("\n\nНажмите любую клавишу, чтобы вернуться в главное меню...")
    c.curs_set(0)
    print("Путь не был найден", file=log)
    stdscr.refresh()
    stdscr.getch()


def path_print(
        stdscr: 'c._CursesWindow',
        path: Path,
        city_names: NamesMapping,
        transport_names: NamesMapping,
        log):
    stdscr.addstr("Путь: \n\t" + city_names[path.start_city])
    print("Путь: \n\t" + city_names[path.start_city], end="", file=log)
    for city, cruise in path:
        cruise_str = "(" + transport_names[cruise.transport_type_id] + ", " + \
                     str(cruise.cruise_time) + ", " + str(cruise.cruise_fare) + ")"
        stdscr.addstr(" -> " + cruise_str + " -> \n\t" + city_names[city])
        print(" -> " + cruise_str + " -> \n\t" + city_names[city], end="", file=log)
    stdscr.addstr("\nВсего будет потрачено " + str(path.time_cost) +
                  " минут, " + str(path.money_cost) +
                  " рублей и будет осуществлено " + str(len(path)) +
                  " прямых рейсов.\n", c.color_pair(3))
    print("\nВсего будет потрачено " + str(path.time_cost) +
          " минут, " + str(path.money_cost) +
          " рублей и будет осуществлено " + str(len(path)) +
          " прямых рейсов.", file=log)
