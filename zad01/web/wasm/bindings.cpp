// Most miedzy algorytmem A* z ../../Cpp/astar.hpp a JavaScriptem (React).
// Kompilacja: npm run wasm  (wymaga Emscriptena: brew install emscripten)

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "../../Cpp/astar.hpp"

using emscripten::val;

int matrix[ROWS][COLS];

// grid: tablica JS z ROWS*COLS liczbami (wiersz po wierszu, od gory)
// zwraca { path: [[x, y], ...], visited: [[x, y], ...] }
val run(val grid, int sx, int sy, int ex, int ey) {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            matrix[i][j] = grid[i * COLS + j].as<int>();

    vector<Point> visited;
    vector<Point> path = aStar({sx, sy}, {ex, ey}, &visited);

    auto toArray = [](const vector<Point> &points) {
        val arr = val::array();
        for (const auto p : points) {
            val pair = val::array();
            pair.call<void>("push", p.x, p.y);
            arr.call<void>("push", pair);
        }
        return arr;
    };

    val result = val::object();
    result.set("path", toArray(path));
    result.set("visited", toArray(visited));
    return result;
}

EMSCRIPTEN_BINDINGS(astar) {
    emscripten::function("run", &run);
    emscripten::constant("ROWS", ROWS);
    emscripten::constant("COLS", COLS);
}
