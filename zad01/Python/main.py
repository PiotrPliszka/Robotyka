matrix = [
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 5, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0]
]


def heurestic(x: tuple[int, int], y: tuple[int, int]) -> float:
    '''
    Heurystyka danego pola
    '''
    return abs(x[0] - y[0]) + abs(x[1] - y[1])


def get_neighbors(node: tuple[int, int], grid: list[list[int]]) -> list[tuple[int, int]]:
    '''
    Lista sąsiadów przesyłanego pola [(...),(...),(...),(...)]
    '''
    neighbors = []
    x, y = node

    # Wysokość i szerokość grida
    rows = len(grid)
    cols = len(grid[0])

    # Lista dopuszczalnych kierunków ruchu
    directions = [(0, 1), (0, -1), (1, 0), (-1, 0)]

    for dx, dy in directions:
        # Przejście na każdego sąsiada
        nx, ny = x + dx, y + dy
        # Warunek wyjścia poza grida
        if 0 <= nx < rows and 0 <= ny < cols:
            # Warunek trafenia na przeszkodę
            if grid[nx][ny] != 5:
                neighbors.append((nx, ny))
    return neighbors


def reconstruct_path(came_from, current):
    # Odtworzenie trasy od mety
    path = [current]

    #
    while current in came_from:
        # Przechodzimy na pole rodzica
        current = came_from[current]
        # Dopisujemy poprzednika do trasy
        path.append(current)
    # Odwrócenie trasy [Meta -> Start]
    path.reverse()
    return path


def a_star(
        grid: list[list[int]],
        start: tuple[int, int],
        goal: tuple[int, int]
) -> list[tuple[int, int]] | None:
    # Lista atrakcyjności pól bez sąsiadów
    open_set = {start}
    # Lista całkowicie zbadanych pól
    closed_set = set()
    # Trasa
    came_from = {}

    # Obecny koszt
    g_score = {start: 0}
    # Szacowany koszt do mety
    f_score = {start: heurestic(start, goal)}

    while open_set:
        current = None
        lowest_f = float('inf')

        for node in open_set:
            node_f = f_score[node]
            if node_f < lowest_f:
                lowest_f = node_f
                current = node

        if current == goal:
            return reconstruct_path(came_from, current)

        open_set.remove(current)
        closed_set.add(current)

        for neighbor in get_neighbors(current, grid):
            if neighbor in closed_set:
                continue
            # Koszt przejścia do sąsiada
            tentative_g = g_score[current] + 1

            if tentative_g < g_score.get(neighbor, float('inf')):
                came_from[neighbor] = current
                g_score[neighbor] = tentative_g
                f_score[neighbor] = tentative_g + heurestic(neighbor, goal)
                open_set.add(neighbor)

    return None


def print_grid_with_path(grid, path, start, goal):
    path_set = set(path) if path else set()

    for r in range(len(grid)):
        row_str = []
        for c in range(len(grid[0])):
            pos = (r, c)
            if pos == start:
                row_str.append("S")
            elif pos == goal:
                row_str.append("G")
            elif pos in path_set:
                row_str.append("·")
            elif grid[r][c] == 5:
                row_str.append("#")
            else:
                row_str.append(" ")
        print(" ".join(row_str))


# --- TEST ---
start_pos = (0, 0)
goal_pos = (19, 19)

path = a_star(matrix, start_pos, goal_pos)

if path:
    print(f"Znaleziono ścieżkę ({len(path)} kroków):\n")
    print_grid_with_path(matrix, path, start_pos, goal_pos)
    print("\nLegenda: S = Start, G = Cel, · = Trasa, # = Ściana")
else:
    print("Brak ścieżki!")