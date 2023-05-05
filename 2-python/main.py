from classes import *
from tui import init_tui
from readf import *
from util import *

import sys


def main(_, file="25_cubes.txt"):
    # if len(argv) < 2:
    #     print("Не предоставлено имя файла, из которого выгружаются данные!")
    #     return
    # file = argv[1]

    g = Graph({})
    cities = NamesMapping()
    transports = NamesMapping()

    with open("log.txt", "a") as log:
        print("starting program...", file=log)
        get_max_memory_usage(log)

        t_readf = timeit_and_log("readf", log=log)(readf)
        print("reading data from file...")
        t_readf(file, cities, transports, g, log)

        get_max_memory_usage(log)
        init_tui(cities, transports, g, log)
        get_max_memory_usage(log)
        print("closing program...", file=log)


if __name__ == '__main__':
    main(*sys.argv)
