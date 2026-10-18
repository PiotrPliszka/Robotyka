#include  <algorithm>
#include  <iostream>
#include <set>
#include <vector>
#include  <fstream>
#include <cmath>

using namespace std;

const int ROWS = 20;
const int COLS = 20;
const int WALL = 5;

struct Punkt {
    int x;
    int y;
    int wartosc;
    double g = INFINITY;
    double f = INFINITY;
    int RodzicX = -1;
    int RodzicY = -1;

    bool operator<(const Punkt& inny) const {
        if (x < inny.x) {
            return true;
        }
        if (x > inny.x) {
            return false;
        }
        if (y < inny.y) {
            return true;
        }
        return false;
    }
};

Punkt siatka[ROWS][COLS];

void wczytaj_siatke(ifstream& plik) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int pole;
            plik >> pole;

            siatka[i][j] = Punkt{i, j};
            siatka[i][j].wartosc = pole;
        }
    }
}

void pokaż_siatke(Punkt siatka[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << siatka[i][j].wartosc << " ";
        }
        cout << '\n';
    }
}

void nanieś_ścieżke(vector<Punkt> ścieżka) {
    for (const Punkt& punkt : ścieżka) {
        siatka[punkt.x][punkt.y].wartosc = 3;
    }
}

double oblicz_f(Punkt obecny, Punkt cel) {
    return sqrt(pow((obecny.x - cel.x), 2) + pow((obecny.y - cel.y), 2));
}

vector<Punkt> znajdz_sąsiadów(Punkt obecny) {
    vector<Punkt> sąsiedzi;
    int x = obecny.x;
    int y = obecny.y;

    int kx[4] = {-1, 1, 0, 0};
    int ky[4] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int tx = x + kx[i];
        int ty = y + ky[i];
        if (tx >= 0 && tx < ROWS && ty >= 0 && ty < COLS) {
            if (siatka[tx][ty].wartosc != WALL) {
                sąsiedzi.push_back(Punkt{tx, ty});
            }
        }
    }
    return  sąsiedzi;
}

vector<Punkt> budowa_ścieżki(Punkt obecny) {
    vector<Punkt> ścieżka;
    while (obecny.x != -1) {
        ścieżka.push_back(obecny);
        Punkt dane = siatka[obecny.x][obecny.y];
        obecny = Punkt{dane.RodzicX, dane.RodzicY};
    }
    reverse(ścieżka.begin(), ścieżka.end());
    return ścieżka;
};

vector<Punkt> a_gwiazdka(Punkt start, Punkt cel) {
    set<Punkt> lista_otwarta = {start};
    set<Punkt> lista_zamknięta = {};
    siatka[start.x][start.y].g = 0;
    siatka[start.x][start.y].f = oblicz_f(start, cel);

    while (!lista_otwarta.empty()) {
        Punkt obecny = *lista_otwarta.begin();
        double najnizszy_f = INFINITY;
        for (const Punkt punkt : lista_otwarta) {
            if (siatka[punkt.x][punkt.y].f <= najnizszy_f) {
                najnizszy_f = siatka[punkt.x][punkt.y].f;
                obecny = punkt;
            }
        }
        if (obecny.x == cel.x && obecny.y == cel.y) {
            return budowa_ścieżki(obecny);
        }
        lista_otwarta.erase(obecny);
        lista_zamknięta.insert(obecny);

        for (const Punkt sąsiad : znajdz_sąsiadów(obecny)) {
            if (lista_zamknięta.count(sąsiad)) {
                continue;
            }
            double koszt = siatka[obecny.x][obecny.y].g + 1;
            Punkt& dane_sąsiad = siatka[sąsiad.x][sąsiad.y];
            if (koszt < dane_sąsiad.g) {
                dane_sąsiad.RodzicX = obecny.x;
                dane_sąsiad.RodzicY = obecny.y;
                dane_sąsiad.g = koszt;
                dane_sąsiad.f = koszt + oblicz_f(sąsiad, cel);
                lista_otwarta.insert(sąsiad);
            }
        }
    }
    return {};
};

int main() {
    ifstream plik("grid.txt");
    if (!plik) {
        cout << "Bład podczas ładowania pliku";
        return 1;
    }
    wczytaj_siatke(plik);
    cout << "Siatka bez wyznaczonej trasy";
    cout << '\n';
    pokaż_siatke(siatka);
    cout << '\n';

    Punkt start = {19, 0};
    Punkt cel = {0, 19};

    vector<Punkt> ścieżka = a_gwiazdka(start, cel);

    if (ścieżka.empty()) {
        cout << "Brak ścieżki \n";
        return 0;
    }
    cout << "Po naniesieniu ścieżki: ";
    cout << '\n';
    nanieś_ścieżke(ścieżka);
    pokaż_siatke(siatka);
}
