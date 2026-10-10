# Robotyka

Zadania z robotyki: implementacje algorytmów planowania ruchu wraz z wizualizacją.

| Zadanie | Temat | Technologie |
|---|---|---|
| [`zad01`](zad01) | Algorytm A\* na siatce 20×20 | C++17 · Python · React + WebAssembly · Docker |

---

## zad01 – Algorytm A\*

### Założenia

| | |
|---|---|
| **Mapa** | `0` – wolne pole, `5` – przeszkoda, `3` – wyznaczona trasa |
| **Start** | (0, 0) – pierwszy wiersz od dołu, pierwsza kolumna |
| **Cel** | (19, 19) – dwudziesty wiersz od dołu, dwudziesta kolumna |
| **Heurystyka** | odległość euklidesowa `√(Δx² + Δy²)` |
| **Koszt ruchu** | 1 |
| **Ruchy** | góra, dół, lewo, prawo (bez skosów), sprawdzane w tej kolejności |

### Struktura

```
zad01/
├── Cpp/
│   ├── astar.hpp          # algorytm A* – wspólny dla wersji konsolowej i web
│   ├── main.cpp           # program konsolowy: mapa z zadania + wydruk macierzy wynikowej
│   ├── CMakeLists.txt
│   ├── grid.txt           # przykładowa mapa z generatora
│   └── ERIA_c1_Astar.pdf  # treść zadania
├── Python/
│   └── main.py            # pierwsza, robocza wersja algorytmu w Pythonie
└── web/
    ├── wasm/bindings.cpp  # most C++ ↔ JavaScript (Emscripten / embind)
    ├── src/               # aplikacja React
    ├── Dockerfile
    └── docker-compose.yml
```

## Jak działa A\*

Dla każdego pola algorytm liczy `f = g + h`:

- **g** – rzeczywisty koszt dojścia od startu (liczba ruchów),
- **h** – heurystyka, czyli szacowana odległość do celu (tu euklidesowa).

W każdym kroku z **listy otwartej** wybierane jest pole o najmniejszym `f`. Trafia ono do **listy zamkniętej**, a jego sąsiedzi (góra, dół, lewo, prawo) dostają zaktualizowane koszty, jeśli znaleziono do nich tańszą drogę. Po dotarciu do celu trasa jest odtwarzana wstecz po rodzicach (`came_from`).

Heurystyka euklidesowa nigdy nie przeszacowuje kosztu ruchu po siatce, więc A\* zawsze znajduje trasę najkrótszą.
