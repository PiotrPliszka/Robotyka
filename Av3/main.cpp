#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

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

    // Zachowanie operatora < w tym strukcie
    bool operator<(const Punkt& inny) const {
        // Najpierw porównujemy wiersze
        if (x < inny.x) {
            return true;
        }
        if (x > inny.x) {
            return false;
        }
        // Wiersze takie same - porównujemy kolumny
        if (y < inny.y) {
            return true;
        }
        return false;
    }
};

// Siatka punktów - musi być pod strukturą
Punkt siatka[ROWS][COLS];

void wczytaj_siatke(ifstream& plik) {
    // Przejście po wszystkich polach mapy
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // Wczytanie wartości pola z pliku
            int pole;
            plik >> pole;

            // Podanie współrzędnych i sprawdzenie czy pole jest ścianą
            siatka[i][j] = Punkt{i, j};
            siatka[i][j].wartosc = pole;
        }
    }
}

void pokaż_siatke(Punkt siatka[ROWS][COLS]) {
    // Przejście po wszystkich polach mapy
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << siatka[i][j].wartosc << " ";
        }
        cout << '\n';
    }
}

void nanieś_ścieżke(vector<Punkt> ścieżka) {
    // Przejście po wszystkich punktach ścieżki
    for (const Punkt& punkt : ścieżka) {
        // Nadpisanie wartości pola w siatce na 3
        siatka[punkt.x][punkt.y].wartosc = 3;
    }
}

double oblicz_f(Punkt obecny, Punkt cel) {
    // Obliczanie wartości f
    return sqrt(pow((obecny.x - cel.x), 2) + pow((obecny.y - cel.y), 2));
}

vector<Punkt> znajdz_sąsiadów(Punkt obecny) {
    // Lista sąsiadów
    vector<Punkt> sąsiedzi;
    // Miejsce podanego punktu
    int x = obecny.x;
    int y = obecny.y;
    // Kierunki poruszania
    int kx[4] = {-1, 1, 0, 0};
    int ky[4] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int tx = x + kx[i];
        int ty = y + ky[i];
        // Sprawdzenie warunku czy wchodzi poza mape
        if (tx >= 0 && ty >= 0 && tx < ROWS && ty < COLS) {
            // Sprawdzenei warunku czy sąsiad jest ściana
            if (!siatka[tx][ty].wartosc != WALL) {
                sąsiedzi.push_back(Punkt{tx, ty});
            }
        }
    }
    return  sąsiedzi;
}

vector<Punkt> budowa_ścieżki(Punkt obecny) {
    vector<Punkt> ścieżka;
    // Pętla działa do momentu kiedy wartosć x nie będzie -1 ponieważ sturct ma tak w defaulcie
    while (obecny.x != -1) {
        // Dodajemy nak koniec
        ścieżka.push_back(obecny);
        // Zmienna pomocnicza kopia pola z siatki do oczytania rodzica
        Punkt dane = siatka[obecny.x][obecny.y];
        // Odczytanie położenia rodzica
        obecny = Punkt{dane.RodzicX, dane.RodzicY};
    }
    // Odwrócenie listy bo idzemy od końca do poczatku
    reverse(ścieżka.begin(), ścieżka.end());
    return ścieżka;
};

vector<Punkt> a_gwiazdka(Punkt start, Punkt cel) {
    set<Punkt> lista_otwarta = {start};
    set<Punkt> lista_zamknięta = {};
    siatka[start.x][start.y].g = 0;
    siatka[start.x][start.y].f = oblicz_f(start, cel);

    // Dopóki lista otwarta nie jest pusta działamy w pętli
    while (!lista_otwarta.empty()) {
        // Iterator odczytuje pierwszą pozycje w liście otwartej
        Punkt obecny = *lista_otwarta.begin();
        double najnizszy_f = INFINITY;
        // Przejście po wszystkich punktach w liście otwartej
        for (const Punkt punkt : lista_otwarta) {
            // ustalenie najniższego f
            if (siatka[punkt.x][punkt.y].f < najnizszy_f) {
                obecny = punkt;
                najnizszy_f = siatka[punkt.x][punkt.y].f;
            }
        }
        if (obecny.x == cel.x && obecny.y == cel.y) {
            // Cel zaleziony
            return budowa_ścieżki(obecny);
        }

        lista_otwarta.erase(obecny);
        lista_zamknięta.insert(obecny);

        // Przechodzimy po wszystkich sąsiadach
        for (const Punkt sąsiad : znajdz_sąsiadów(obecny)) {
            if (lista_zamknięta.count(sąsiad)) {
                continue;
            }
            // Obliczenie kosztu przejscia
            double koszt = siatka[obecny.x][obecny.y].g + 1;
            // Utworzenie bezpośredniego połączenia z polem w siatce
            Punkt& dane_sąsiad = siatka[sąsiad.x][sąsiad.y];
            if (koszt < dane_sąsiad.g) {
                // Modyfikacja danych
                dane_sąsiad.RodzicX = obecny.x;
                dane_sąsiad.RodzicY = obecny.y;
                dane_sąsiad.g = koszt;
                dane_sąsiad.f = koszt + oblicz_f(sąsiad, cel);
                // Dodanie pola do listy otwartej
                lista_otwarta.insert(sąsiad);
            }
        }
    }
    return {};
}

int main() {
    // Ładowanie pliku
    ifstream plik("grid.txt");
    if (!plik) {
        // Komunikat o błędzie ładowania
        cout << "Błąd podczas ładowania pliku";
        return 1;
    }
    // Wczytanie siatki
    wczytaj_siatke(plik);
    // Pokazanie siatki
    cout << "Siatka bez wyznaczonej trasy:";
    cout << '\n';
    pokaż_siatke(siatka);
    cout << '\n';


    Punkt start = {19, 0};
    Punkt cel = {0, 19};


    vector<Punkt> ścieżka = a_gwiazdka(start, cel);

    if (ścieżka.empty()) {
        cout << "Brak sciezki\n";
        return 0;
    }
    cout << "Po naniesieniu ścieżki: ";
    cout << '\n';
    nanieś_ścieżke(ścieżka);
    pokaż_siatke(siatka);
}
