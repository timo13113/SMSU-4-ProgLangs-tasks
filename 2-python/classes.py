from dataclasses import dataclass, field
from typing import Tuple, Dict, Set, Union, Final
import heapq as h

INF: Final = float('inf')
IntOrInf = Union[int, float]


@dataclass
class NamesMapping:
    str_to_idx: dict[str, int] = field(default_factory=dict)
    idx_to_str: list[str] = field(default_factory=list)

    def __getitem__(self, item):
        return self.idx_to_str[item]

    def __len__(self):
        assert len(self.str_to_idx) == len(self.idx_to_str), "bad contents size!!!"
        return len(self.str_to_idx)

    def get(self, s: str):
        return self.str_to_idx[s]

    def has(self, s: str):
        return s in self.str_to_idx

    def add_else_get(self, s: str):
        ret = self.str_to_idx.setdefault(s, len(self.idx_to_str))
        if ret == len(self.idx_to_str):
            self.idx_to_str.append(s)
        return ret


@dataclass(eq=True, frozen=True, slots=True)
class Cruise:
    # __slots__ = ['transport_type_id', 'cruise_time', 'cruise_fare']
    transport_type_id: int = 0
    cruise_time: int = 0
    cruise_fare: int = 0


@dataclass(eq=True, frozen=True)
class Path:
    start_city: int = None
    transfer_cities: Union[list[int], None] = None
    cruises: Union[list[Cruise], None] = None
    time_cost: IntOrInf = 0
    money_cost: IntOrInf = 0
    n_tickets: IntOrInf = 0

    def __post_init__(self):
        # пустая инициализация - сделать стоимость бесконечной
        if self.transfer_cities is None:
            # self.time_cost = INF
            # self.money_cost = INF
            # self.n_tickets = INF
            object.__setattr__(self, 'time_cost', INF)
            object.__setattr__(self, 'money_cost', INF)
            object.__setattr__(self, 'n_tickets', INF)
        # если вектор пустой, то стоимость 0, иначе
        else:
            for x in self.cruises:
                # self.time_cost += x.cruise_time
                # self.money_cost += x.cruise_fare
                # self.n_tickets += 1
                object.__setattr__(self, 'time_cost', self.time_cost + x.cruise_time)
                object.__setattr__(self, 'money_cost', self.money_cost + x.cruise_fare)
                object.__setattr__(self, 'n_tickets', self.n_tickets + 1)

    def __add__(self, other: Tuple[Cruise, int]):
        new_cruises: list[Cruise] = self.cruises[:]
        new_cruises.append(other[0])
        new_transfer_cities: list[int] = self.transfer_cities[:]
        new_transfer_cities.append(other[1])
        return Path(self.start_city, new_transfer_cities, new_cruises)

    def __getitem__(self, i):
        return self.transfer_cities[i], self.cruises[i]

    def __len__(self):
        assert len(self.transfer_cities) == len(self.cruises), "bad contents size!!!"
        return len(self.transfer_cities)

    def __iter__(self):
        return zip(self.transfer_cities, self.cruises)


@dataclass
class Graph:
    graph: Dict[int, Dict[int, Set[Cruise]]]

    def has_edges(self, id_from: int, id_to: int) -> Union[Set[Cruise], None]:
        if id_from in self.graph:
            if id_to in self.graph[id_from]:
                return self.graph[id_from][id_to]
        return None

    def add_node(self, id_from: int, id_to: int, cruise: Cruise):
        if id_from in self.graph:
            if id_to in self.graph[id_from]:
                self.graph[id_from][id_to].add(cruise)
            else:
                self.graph[id_from][id_to] = {cruise}
        else:
            self.graph[id_from] = {id_to: {cruise}}

    def sources(self):
        return self.graph.keys()

    def __getitem__(self, item: int) -> Dict[int, Set[Cruise]]:
        return self.graph[item]


@dataclass
class SinglePathMapping:
    source_id: int
    n: int
    instruction: int
    paths: Dict[int, Union[Path, None]] = field(default_factory=dict)
    used: list[int] = field(default_factory=list)
    distances: list[Union[
        Tuple[IntOrInf, int], Tuple[IntOrInf, IntOrInf, int]
    ]] = field(default_factory=list)

    def __post_init__(self):
        # self.paths = [None] * self.n
        self.paths = {i: None for i in range(self.n)}

        self.paths[self.source_id] = Path(self.source_id, [], [])
        if self.instruction == 1:
            self.distances = [(0, 0, self.source_id)]
        else:
            self.distances = [(0, self.source_id)]
        h.heapify(self.distances)
        self.used = []

    def __getitem__(self, item) -> Path:
        return self.paths[item]

    def push(self, path: Path):
        destination = path.transfer_cities[-1]
        if self.instruction == 1:
            h.heappush(self.distances,
                       (path.time_cost, path.money_cost, destination)
                       )
        elif self.instruction == 2 or self.instruction == 4:
            h.heappush(self.distances,
                       (path.money_cost, destination)
                       )
        elif self.instruction == 5:
            h.heappush(self.distances,
                       (path.time_cost, destination)
                       )
        else:
            h.heappush(self.distances,
                       (path.n_tickets, destination)
                       )
        self.paths[destination] = path
        # print("pushing to", destination)

    def pop(self):
        if not self.distances:
            return [None]
        ret = h.heappop(self.distances)
        destination: int = ret[-1]
        self.used.append(destination)
        if self.distances:
            while destination == self.distances[0][-1]:
                h.heappop(self.distances)
        return ret
