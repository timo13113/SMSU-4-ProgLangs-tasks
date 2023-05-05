from classes import *
from util import *


def readf(
        filename: str, city_names: NamesMapping, transport_names: NamesMapping, graph: Graph, logs=None
):
    counter = 0
    with open(filename, "r", encoding='UTF-8') as f:
        for line in f:
            if (i := line.find('#')) != -1:
                line = line[:i]
            line = line.strip()
            if len(line) == 0:
                continue
            counter += 1

            objs = line.split('"')
            print(objs, file=logs)

            from_city, to_city, transport_type = objs[1], objs[3], objs[5]
            cruise_time, cruise_fare = [int(j) for j in objs[-1].split(' ')[1:]]

            id_from = city_names.add_else_get(from_city)
            id_to = city_names.add_else_get(to_city)
            id_transport = transport_names.add_else_get(transport_type)

            graph.add_node(id_from, id_to, Cruise(id_transport, cruise_time, cruise_fare))

    print("processed total", counter, "lines", file=logs)
    return 0


# @timeit_and_log("readf", log=None)
# def timed_readf(*args, **kwargs):
#     return readf(*args, **kwargs)
