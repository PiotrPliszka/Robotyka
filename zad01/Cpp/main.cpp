#include <algorithm>
#include <climits>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

const int ROWS = 20;
const int COLS = 20;
const int WALL = 5;

int matrix[ROWS][COLS] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 5, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0}
};

struct Point {
    int x;
    int y;

    bool operator<(const Point other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }

    bool operator==(const Point other) const {
        return x == other.x && y == other.y;
    }
};

double heurestic(Point a, Point b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

vector<Point> get_neighbours(Point node) {
    vector<Point> neighbors;
    int x = node.x;
    int y = node.y;

    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = { 0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < ROWS && ny >= 0 && ny < COLS) {
            if (matrix[nx][ny] != WALL) {
                neighbors.push_back({nx, ny});
            }
        }
    }
    return neighbors;
}

vector<Point> reconstructPath(map<Point, Point> came_from, Point current) {
    vector<Point> path;
    path.push_back(current);

    while (came_from.find(current) != came_from.end()) {
        current = came_from[current];
        path.push_back(current);
    }

    reverse(path.begin(), path.end());
    return path;
}

vector<Point> aStar(Point start, Point end) {
    set<Point> open_set = {start};
    set<Point> closed_set = {};
    map<Point, Point> came_from;
    map<Point, int> g_score;
    map<Point, double> f_score;
    g_score[start] = 0;
    f_score[start] = heurestic(start, end);

    while (!open_set.empty()) {
        Point current;
        double lowest_f = INFINITY;

        for (const auto node : open_set) {
            double node_f = f_score.count(node) ? f_score[node] : INFINITY;
            if (node_f < lowest_f) {
                lowest_f = node_f;
                current = node;
            }
        }

        if (current.x == end.x && current.y == end.y) {
            return reconstructPath(came_from, current);
        }

        open_set.erase(current);
        closed_set.insert(current);

        for (const auto neighbour : get_neighbours(current)) {
            if (closed_set.count(neighbour)) {
                continue;
            }

            int tentative_g = g_score[current] + 1;
            int neighbour_g = g_score.count(neighbour) ? g_score[neighbour] : INT_MAX;

            if (tentative_g < neighbour_g) {
                came_from[neighbour] = current;
                g_score[neighbour] = tentative_g;
                f_score[neighbour] = tentative_g + heurestic(neighbour, end);
                open_set.insert(neighbour);
            }
        }
    }
    return {};
}

void printMatrix(const int grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << grid[i][j] << " ";
        }
        cout << "\n";
    }
}

int main() {
    // macierz jest zapisana od gory, wiec wiersz "pierwszy od dolu" to ROWS - 1
    Point start = {ROWS - 1, 0};      // (0,0) z zadania: lewy dolny rog
    Point end = {0, COLS - 1};        // (19,19) z zadania: prawy gorny rog

    vector<Point> path = aStar(start, end);

    if (path.empty()) {
        cout << "Nie znaleziono sciezki!\n";
        printMatrix(matrix);
        return 0;
    }

    int result[ROWS][COLS];
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            result[i][j] = matrix[i][j];

    for (const auto p : path) {
        result[p.x][p.y] = 3;
    }

    printMatrix(result);
    return 0;
}