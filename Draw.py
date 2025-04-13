import matplotlib.pyplot as plt


class Edge:
    def __init__(self, p1: tuple, p2: tuple):
        self.p1 = p1
        self.p2 = p2


class Triangulation:
    def __init__(self, path_to_file : str):
        flag = 1
        curr_edge = Edge((), ())
        edges = []

        # Считываем данные из файла. В результате в edges будут лежать все мосты
        with open("points.txt") as points_file:
            for point in points_file:
                x, y = tuple(map(float, point.strip().split()))

                if flag == 1:
                    curr_edge.p1 = (x, y)
                    flag = 2

                elif flag == 2:
                    curr_edge.p2 = (x, y)
                    edges.append(curr_edge)
                    curr_edge = Edge((), ())  # Конец моста, добавляем мост в список, создаем новый пустой мост
                    flag = 1

        self.edges = edges

    def build_graph(self):
        for edge in self.edges:
            # edge.p[1, 2] = (x, y)
            x = [edge.p1[0], edge.p2[0]]
            y = [edge.p1[1], edge.p2[1]]
            plt.plot(x, y, color='black', marker='o', markersize=7)

        plt.xlabel('X')
        plt.ylabel('Y')
        plt.title('Triangulation')
        plt.show()


if __name__ == '__main__':
    triangulation = Triangulation('pathtofile')
    triangulation.build_graph()
