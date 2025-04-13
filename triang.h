#ifndef TRIANG_H
#define TRIANG_H

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <cmath>

/**
 * @struct Point
 * @brief Точка в 2D-пространстве с координатами x и y.
 */
struct Point {
    double x, y;

    /**
     * @brief Оператор сравнения для сортировки точек.
     * @param other Другая точка для сравнения.
     * @return true, если текущая точка меньше other.
     */
    bool operator<(const Point& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }

    /**
     * @brief Проверка на неравенство точек.
     * @param other Другая точка для сравнения.
     * @return true, если точки не равны.
     */
    bool operator!=(const Point& other) const {
        return (x != other.x) || (x == other.x && y != other.y);
    }

    /**
     * @brief Проверка на равенство точек.
     * @param other Другая точка для сравнения.
     * @return true, если точки равны.
     */
    bool operator==(const Point& other) const {
        return (x == other.x && y == other.y);
    }
};

/**
 * @brief Возвращает минимальную из двух точек (по координатам).
 * @param a Первая точка.
 * @param b Вторая точка.
 * @return Точка с меньшими координатами.
 */
Point min(Point a, Point b) {
    if (a < b) { return a; }
    return b;
}

/**
 * @brief Возвращает максимальную из двух точек (по координатам).
 * @param a Первая точка.
 * @param b Вторая точка.
 * @return Точка с большими координатами.
 */
Point max(Point a, Point b) {
    if (b < a) { return a; }
    return b;
}

/**
 * @class Edge
 * @brief Ребро, соединяющее две точки (p1 и p2).
 */
class Edge {
public:
    Point p1, p2;

    /**
     * @brief Конструктор ребра.
     * @param point1 Первая точка.
     * @param point2 Вторая точка.
     */
    Edge(const Point& point1, const Point& point2) : p1(point1), p2(point2) {}

    /**
     * @brief Оператор сравнения для сортировки рёбер.
     * @param other Другое ребро.
     * @return true, если текущее ребро меньше other.
     */
    bool operator<(const Edge& other) const {
        return (p1 < other.p1) || (p1 == other.p1 && p2 < other.p2);
    }

    /**
     * @brief Проверка на равенство рёбер (без учёта порядка точек).
     * @param other Другое ребро.
     * @return true, если рёбра равны.
     */
    bool operator==(const Edge& other) const {
        return (p1 == other.p1 && p2 == other.p2) || (p1 == other.p2 && p2 == other.p1);
    }
};

/**
 * @brief Записывает рёбра в файл "points.txt".
 * @param edges Множество рёбер для записи.
 */
void writeEdgesToFile(const std::set<Edge>& edges) {
    std::ofstream outFile("points.txt");
    if (!outFile) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return;
    }

    for (const auto& edge : edges) {
        outFile << edge.p1.x << " " << edge.p1.y << std::endl;
        outFile << edge.p2.x << " " << edge.p2.y << std::endl;
    }

    outFile.close();
}

/**
 * @brief Определяет ориентацию трёх точек.
 * @param p Первая точка.
 * @param q Вторая точка.
 * @param r Третья точка.
 * @return 1, если по часовой стрелке, 2 — против.
 */
int orientation(Point p, Point q, Point r) {
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    return (val > 0) ? 1 : 2;
}

/**
 * @brief Проверяет, пересекает ли ребро e отрезок между p1 и p2.
 * @param e Ребро.
 * @param p1 Первая точка отрезка.
 * @param p2 Вторая точка отрезка.
 * @return 1, если пересекает, 0 — если нет.
 */
int cross(Edge e, Point p1, Point p2) {
    if (p1 == e.p1 || p1 == e.p2 || p2 == e.p2 || p2 == e.p1) { return 0; }

    int o1 = orientation(e.p1, e.p2, p1);
    int o2 = orientation(e.p1, e.p2, p2);
    int o3 = orientation(p1, p2, e.p1);
    int o4 = orientation(p1, p2, e.p2);

    if (o1 != o2 && o3 != o4) {
        return 1;
    }
    return 0;
}

/**
 * @brief Триангуляция множества точек.
 * @param points Вектор точек для триангуляции.
 * @return Множество рёбер триангуляции.
 */
std::set<Edge> triangulate(std::vector<Point>& points) {
    std::sort(points.begin(), points.end());
    std::set<Edge> edges;
    Edge e = {{0, 0}, {0, 0}};
    std::size_t t = points.size();
    std::set<Point> ourpoints;
    int flag = 0;

    if (t == 1) { return edges; }
    if (t == 2) {
        e.p1 = points[0];
        e.p2 = points[1];
        edges.insert(e);
        return edges;
    }

    // Первый треугольник
    Point a = points[0];
    Point b = points[1];
    Point c = points[2];
    edges.insert({min(a, b), max(a, b)});
    edges.insert({min(b, c), max(b, c)});
    edges.insert({min(a, c), max(a, c)});
    ourpoints.insert(a);
    ourpoints.insert(b);
    ourpoints.insert(c);

    // Добавление остальных точек
    for (int i = 3; i < t; ++i) {
        Point p = points[i];
        for (const auto& point : ourpoints) {
            flag = 0;
            for (const auto& edge : edges) {
                if (cross(edge, point, p) == 1) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                edges.insert({min(p, point), max(p, point)});
            }
        }
        ourpoints.insert(p);
    }
    return edges;
}


#endif // TRIANG_H