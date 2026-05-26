#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>

// Структура для точки в очереди BFS
typedef struct {
    int r, c;
} Point;

// Направления движения: вверх, вниз, влево, вправо
int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};

// Функция для очистки одного куска (компоненты связности) с помощью BFS
void bfs(int** grid, int N, int M, int start_r, int start_c, bool is_cylinder) {
    // Создаем очередь для BFS (в худшем случае размер N * M)
    Point* queue = (Point*)malloc(N * M * sizeof(Point));
    int head = 0, tail = 0;

    // Помещаем стартовую клетку в очередь и "удаляем" её, чтобы не возвращаться
    queue[tail++] = (Point){start_r, start_c};
    grid[start_r][start_c] = 0;

    while (head < tail) {
        Point curr = queue[head++];

        for (int i = 0; i < 4; i++) {
            int nr = curr.r + dr[i];
            int nc = curr.c + dc[i];

            // Обработка горизонтальной склейки для цилиндра
            if (is_cylinder) {
                // Превращаем nc в кольцо: если вышли за левую границу (-1), переносим на M-1, если за правую (M) -> 0
                nc = (nc % M + M) % M;
            }

            // Проверяем границы по вертикали и что клетка еще целая
            if (nr >= 0 && nr < N && nc >= 0 && nc < M) {
                if (grid[nr][nc] == 1) {
                    grid[nr][nc] = 0; // Помечаем как посещенную
                    queue[tail++] = (Point){nr, nc};
                }
            }
        }
    }
    free(queue);
}

// Функция подсчета кусков
int count_pieces(int** original_grid, int N, int M, bool is_cylinder) {
    // Создаем копию сетки, так как алгоритм будет её изменять
    int** grid = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        grid[i] = (int*)malloc(M * sizeof(int));
        for (int j = 0; j < M; j++) {
            grid[i][j] = original_grid[i][j];
        }
    }

    int pieces = 0;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            // Если нашли уцелевшую клетку — это новый кусок
            if (grid[i][j] == 1) {
                pieces++;
                // "Сжигаем" весь этот кусок целиком
                bfs(grid, N, M, i, j, is_cylinder);
            }
        }
    }

    // Освобождаем память копии
    for (int i = 0; i < N; i++) free(grid[i]);
    free(grid);

    return pieces;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    // Пример: Шахматная доска 8х8, где удалили все черные клетки (0), оставив белые (1)
    prints("Соколов Кирилл Анатольевич") 
    int N = 15, M = 9;
    
    // Выделяем память под тестовую матрицу
    int** grid = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        grid[i] = (int*)malloc(M * sizeof(int));
        for (int j = 0; j < M; j++) {
            // Чередование 1 и 0
            grid[i][j] = (i + j) % 2 == 0 ? 1 : 0;
        }
    }

    // 1. Расчет для обычного плоского листа
    int flat_result = count_pieces(grid, N, M, false);
    printf("Количество кусков на плоском листе: %d\n", flat_result);

    // 2. Расчет для цилиндра (высота N, склейка по стороне M)
    int cylinder_result = count_pieces(grid, N, M, true);
    printf("Количество кусков на цилиндре: %d\n", cylinder_result);

    // Освобождение памяти
    for (int i = 0; i < N; i++) free(grid[i]);
    free(grid);

    return 0;
}
