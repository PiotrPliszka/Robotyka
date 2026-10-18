#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <iostream>
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

struct Punkt {
    int x;
    int y;
    mutable int g = INT_MAX;
    mutable double f = INFINITY;
    int rodzic_x = -1;
    int rodzic_y = -1;

    bool operator<(const Punkt inny) const {
        if (x != inny.x) return x < inny.x;
        return y < inny.y;
    }
};


double obliczanie_f(Punkt poz, Punkt cel) {
    // Obliczanie wartości f dla przesłanego punktu
    return sqrt(pow((poz.x - cel.x), 2) + pow((poz.y - cel.y), 2));
}

vector<Punkt> znajdz_sąsiadów(Punkt poz) {
    // List sąsiadów
    vector<Punkt> sąsiad;

    // Pozycja przesłanego punktu
    int x = poz.x;
    int y = poz.y;

    // Możliwe kierunki poruszania
    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = { 0, 0, -1, 1};

    // Przejście po wszystkich kierunkach
    for (int i = 0; i < 4; i++) {
        int sx = x + dx[i];
        int sy = y + dy[i];

        // Warunek sprawdzający czy sąsiednie pole nie wychodzi poza macierz
        if (sx >= 0 && sy >= 0 && sx < ROWS && sy < COLS) {

            // Warunek sprawdzający czy sąsiednie pole nie jestes ścianą
            if (matrix[sx][sy] != WALL) {
                sąsiad.push_back(Punkt{sx, sy});
            }
        }
    }
    return sąsiad;
}

vector<Punkt> budowa_ścieżki(Punkt obecny) {
    vector<Punkt> sciezka;

    // Cofamy się po rodzicach od celu aż do startu (start ma rodzica -1, -1)
    while (obecny.x != -1) {
        sciezka.push_back(obecny);
        Punkt& dane = siatka[obecny.x][obecny.y];
        obecny = Punkt{dane.rodzic_x, dane.rodzic_y};
    }

    // Odwracamy, żeby ścieżka szła od startu do celu
    reverse(sciezka.begin(), sciezka.end());
    return sciezka;
};

vector<Punkt> a_star(Punkt start, Punkt end) {
    set<Punkt> otwarta_lista = {start};
    set<Punkt> zamknięta_lista = {};
    start.g = 0;
    start.f = obliczanie_f(start, end);

    while (!otwarta_lista.empty()) {
        // Punkt pomocniczy
        Punkt obecny;
        // Zmienna Pomocnicza
        double najmniejsza_f = INFINITY;

        // Przejście po każdym strucie w open secie
        for (const auto punkt : otwarta_lista) {
            // Obliczanie dla kazdego punktu wartości f
            punkt.f = obliczanie_f(punkt, end);
            // Wybieramy najmniejsza wartość_f z open seta
            if (punkt.f < najmniejsza_f) {
                najmniejsza_f = punkt.f;
                obecny = punkt;
            }
        }
        if (obecny.x == end.x && obecny.y == end.y) {
            // Punkt końcowy zanelziony budowa ścieżki
        }
        // Usunięcie z listy otwartej
        otwarta_lista.erase(obecny);
        // Dodanie do listy zamkniętej
        zamknięta_lista.insert(obecny);

        // Przechodzimy po wszystkich sąsiadach punktu
        for (const auto sąsiad : znajdz_sąsiadów(obecny)) {
            // Jeżeli punkt w liscie zamknietej koniec
            if (zamknięta_lista.count(sąsiad)) {
                continue;
            }
            // Kosz przejścia do sąsiada obecny + 1
            int koszt_przejścia = obecny.g + 1;

            // Jeżeli koszt przejścia do sąsiada jest mniejszy niż wcześniej lub jeśli go nie ma obliczamy wartości
            if (koszt_przejścia < sąsiad.g) {
                // Podanie rodzica
                sąsiad.rodzic = &obecny;
                // Podanie kosztu przejścia
                sąsiad.g = koszt_przejścia;
                // Obliczanie wartości f
                sąsiad.f = koszt_przejścia + obliczanie_f(sąsiad, end);
                // Dodanie do listy otwartej
                otwarta_lista.insert(sąsiad);
            }
        }
    }
    return {};
}


int main(int argc, char *argv[]) {
    Punkt pozycja_startowa = {19, 0};
    Punkt pozycja_końcowa = {0, 19};

    vector<Punkt> ścieżka = a_star(pozycja_startowa, pozycja_końcowa);

};

