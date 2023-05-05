from classes import *
from typing import List
from util import *


# @timeit_and_log("dijkstra", log=None)
# def timed_dijkstra(*args, **kwargs):
#     return dijkstra(*args, **kwargs)


def dijkstra(
    from_id: int,
    to_id: int,
    limit: int,
    graph: Graph,
    transport_whitelist: List[bool],
    n: int,
    instruction: int
) -> SinglePathMapping:

    paths: SinglePathMapping = SinglePathMapping(from_id, n, instruction)

    if instruction == 1:
        def halting(_v):
            # дошли куда надо
            return _v[-1] == to_id

        def is_better(a, e: Cruise, b):
            if paths[a].time_cost + e.cruise_time == paths[b].time_cost:
                return paths[a].money_cost + e.cruise_fare < paths[b].money_cost
            else:
                return paths[a].time_cost + e.cruise_time < paths[b].time_cost
    elif instruction == 2:
        def halting(_v):
            # дошли куда надо
            return _v[-1] == to_id

        def is_better(a, e: Cruise, b):
            return paths[a].money_cost + e.cruise_fare < paths[b].money_cost
    elif instruction == 3:
        def halting(_v):
            # дошли куда надо
            return _v[-1] == to_id

        def is_better(a, _, b):
            return paths[a].n_tickets + 1 < paths[b].n_tickets
    elif instruction == 4:
        def halting(_v):
            # превысили предел
            return _v[0] > limit

        def is_better(a, e: Cruise, b):
            return paths[a].money_cost + e.cruise_fare < paths[b].money_cost
    else:  # instruction == 5:
        def halting(_v):
            # превысили предел
            return _v[0] > limit

        def is_better(a, e: Cruise, b):
            return paths[a].time_cost + e.cruise_time < paths[b].time_cost

    while True:
        v = paths.pop()
        if v[-1] is None:
            # прошли все достижимые вершины
            # print("reached nothing")
            break
        if halting(v):
            # print("reached destination")
            break
        v = v[-1]
        # print("looking at:", v)
        if v in graph.sources():
            for destination, edges in graph[v].items():
                for edge in edges:
                    if transport_whitelist[edge.transport_type_id]:
                        if paths[destination] is None \
                                or is_better(v, edge, destination):
                            paths.push(paths[v] + (edge, destination))

    return paths
