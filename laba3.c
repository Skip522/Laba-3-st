#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <iomanip>
#include <windows.h>

// Данные об авторе (ВЫВОДЯТСЯ ПРИ СТАРТЕ ПРОГРАММЫ)
const std::string AUTHOR_FIO = "Соколов Кирилл Анатольевич";
const std::string AUTHOR_GROUP = "ИВТ-11"; // Укажи свою группу, если отличается

// Структура точки для BFS
struct Point {
    int r, c;
};

// ============================================================================
// А) РЕАЛИЗАЦИЯ ОЧЕРЕДИ ЧЕРЕЗ ДИНАМИЧЕСКИЙ МАССИВ
// ============================================================================
class ArrayQueue {
private:
    Point* data;
    int head;
    int tail;
    int capacity;
public:
    ArrayQueue(int max_size) {
        capacity = max_size;
        data = new Point[capacity];
        head = 0;
        tail = 0;
    }
    ~ArrayQueue() { delete[] data; }

    void push(Point p) {
        if (tail < capacity) {
            data[tail++] = p;
        }
    }
    void pop() {
        if (head < tail) head++;
    }
    Point front() { return data[head]; }
    bool empty() { return head == tail; }
};

// ============================================================================
// Б) РЕАЛИЗАЦИЯ ОЧЕРЕДИ ЧЕРЕЗ СВЯЗАННЫЙ СПИСОК
// ============================================================================
class ListQueue {
private:
    struct Node {
        Point pt;
        Node* next;
        Node(Point p) : pt(p), next(nullptr) {}
    };
    Node* head;
    Node* tail;
public:
    ListQueue() : head(nullptr), tail(nullptr) {}
    ~ListQueue() {
        while (!empty()) pop();
    }

    void push(Point p) {
        Node* newNode = new Node(p);
        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    void pop() {
        if (head == nullptr) return;
        Node* temp = head;
        head = head->next;
        if (head == nullptr) tail = nullptr;
        delete temp;
    }
    Point front() { return head->pt; }
    bool empty() { return head == nullptr; }
};

// ============================================================================
// В) РЕАЛИЗАЦИЯ С ИСПОЛЬЗОВАНИЕМ СТАНДАРТНОЙ БИБЛИОТЕКИ (STL)
// ============================================================================
class STLQueue {
private:
    std::queue<Point> q;
public:
    void push(Point p) { q.push(p); }
    void pop() { q.pop(); }
    Point front() { return q.front(); }
    bool empty() { return q.empty(); }
};

// ============================================================================
// ШАБЛОННЫЙ АЛГОРИТМ BFS (принимает любой тип очереди)
// ============================================================================
int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};

template <typename QueueType>
void bfs(std::vector<std::vector<int>>& grid, int N, int M, int start_r, int start_c, bool is_cylinder, QueueType& q) {
    q.push({start_r, start_c});
    grid[start_r][start_c] = 0;

    while (!q.empty()) {
        Point curr = q.front();
        q.pop();

        for (int i = 0; i < 4; i++) {
            int nr = curr.r + dr[i];
            int nc = curr.c + dc[i];

            if (is_cylinder) {
                nc = (nc % M + M) % M; // Зацикливание по горизонтали
            }

            if (nr >= 0 && nr < N && nc >= 0 && nc < M) {
                if (grid[nr][nc] == 1) {
                    grid[nr][nc] = 0;
                    q.push({nr, nc});
                }
            }
        }
    }
}

// Функции-обертки для подсчета с разными очередями
int count_pieces_array(const std::vector<std::vector<int>>& orig_grid, int N, int M, bool is_cylinder) {
    auto grid = orig_grid;
    int pieces = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (grid[i][j] == 1) {
                pieces++;
                ArrayQueue q(N * M);
                bfs(grid, N, M, i, j, is_cylinder, q);
            }
        }
    }
    return pieces;
}

int count_pieces_list(const std::vector<std::vector<int>>& orig_grid, int N, int M, bool is_cylinder) {
    auto grid = orig_grid;
    int pieces = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (grid[i][j] == 1) {
                pieces++;
                ListQueue q;
                bfs(grid, N, M, i, j, is_cylinder, q);
            }
        }
    }
    return pieces;
}

int count_pieces_stl(const std::vector<std::vector<int>>& orig_grid, int N, int M, bool is_cylinder) {
    auto grid = orig_grid;
    int pieces = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (grid[i][j] == 1) {
                pieces++;
                STLQueue q;
                bfs(grid, N, M, i, j, is_cylinder, q);
            }
        }
    }
    return pieces;
}

// ============================================================================
// ОСНОВНАЯ ПРОГРАММА
// ============================================================================
int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    // Вывод информации об авторе
    std::cout << "==================================================\n";
    std::cout << "Автор: " << AUTHOR_FIO << "\n";
    std::cout << "Группа: " << AUTHOR_GROUP << "\n";
    std::cout << "==================================================\n\n";

    // Инициализация большой матрицы для нагрузочного тестирования производительности
    int N = 500, M = 500;
    std::vector<std::vector<int>> grid(N, std::vector<int>(M));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            grid[i][j] = (i + j) % 2 == 0 ? 1 : 0; // Шахматный порядок
        }
    }

    std::cout << "Тестирование на сетке " << N << "x" << M << " (Шахматный порядок):\n\n";

    // --- ТЕСТ А: Массив ---
    auto start = std::chrono::high_resolution_clock::now();
    int res_flat_arr = count_pieces_array(grid, N, M, false);
    int res_cyl_arr  = count_pieces_array(grid, N, M, true);
    auto end = std::chrono::high_resolution_clock::now();
    double time_arr = std::chrono::duration<double, std::milli>(end - start).count();

    // --- ТЕСТ Б: Список ---
    start = std::chrono::high_resolution_clock::now();
    int res_flat_list = count_pieces_list(grid, N, M, false);
    int res_cyl_list  = count_pieces_list(grid, N, M, true);
    end = std::chrono::high_resolution_clock::now();
    double time_list = std::chrono::duration<double, std::milli>(end - start).count();

    // --- ТЕСТ В: STL ---
    start = std::chrono::high_resolution_clock::now();
    int res_flat_stl = count_pieces_stl(grid, N, M, false);
    int res_cyl_stl  = count_pieces_stl(grid, N, M, true);
    end = std::chrono::high_resolution_clock::now();
    double time_stl = std::chrono::duration<double, std::milli>(end - start).count();

    // Вывод результатов работы алгоритма
    std::cout << "Результаты вычислений:\n";
    std::cout << "  - Плоский лист кусков: " << res_flat_arr << "\n";
    std::cout << "  - Цилиндр кусков:      " << res_cyl_arr << "\n\n";

    // Сравнительная таблица скоростей
    std::cout << "Сравнение производительности (Flat + Cylinder вместе):\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "| Реализация очереди       | Время выполнения, мс|\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "| А) Динамический массив   | " << std::setw(19) << std::fixed << std::setprecision(2) << time_arr << " |\n";
    std::cout << "| Б) Связанный список      | " << std::setw(19) << time_list << " |\n";
    std::cout << "| В) Стандартная библиотека| " << std::setw(19) << time_stl << " |\n";
    std::cout << "--------------------------------------------------\n";

    return 0;
}
