#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define M_PI 3.14159265358979323846

typedef struct {
    char* name;
    void (*sort_func)(double[], int, int);
} SortInfo;

void swap(double* a, double* b) {
    double t = *a;
    *a = *b;
    *b = t;
}

//Пузырьковая сортировака
void bubble_sort(double arr[], int low, int high) {
    for (int i = low; i <= high; i++) {
        for (int j = low; j < high - (i - low); j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

//Сортировка вставками
void insertion_sort(double arr[], int low, int high) {
    for (int i = low + 1; i <= high; i++) {
        double key = arr[i];
        int j = i - 1;
        while (j >= low && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

//Сортировка выбором
void selection_sort(double arr[], int low, int high) {
    for (int i = low; i < high; i++) {
        int min_idx = i;
        for (int j = i + 1; j <= high; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        swap(&arr[min_idx], &arr[i]);
    }
}

//Разделение для быстрой сортировки
int partition(double arr[], int low, int high) {
    double pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

//Быстрая сортировка
void quicksort(double arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

//Графический вывод функций (ASCII-график)
void plot_signals(double* original, double* noisy, double* filtered, int N, const char* sort_name) {
    int width = 80;
    int height = 20;
    
    printf("\n╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║ Графический вывод: %s %-40s ║\n", sort_name, "");
    printf("╚════════════════════════════════════════════════════════════════════════════════╝\n");
    
    char canvas[3][height][width + 1];
    
    //Инициализация канвас
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                canvas[k][i][j] = ' ';
            }
            canvas[k][i][width] = '\0';
        }
    }
    
    //Функции для рисования
    double* signals[] = {original, noisy, filtered};
    const char* titles[] = {"Оригинальный сигнал", "Зашумлённый сигнал", "Отфильтрованный сигнал"};
    char symbols[] = {'*', '*', '*'};
    
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < N; i++) {
            int x = (i * width) / N;
            if (x >= width) x = width - 1;
            
            //Нормализуем значение в диапазон [0,-1]
            int y = (int)((signals[k][i] + 1.0) * (height - 1) / 2.0);
            if (y < 0) y = 0;
            if (y >= height) y = height - 1;
            
            canvas[k][height - 1 - y][x] = symbols[k];
        }
        
        printf("\n%s:\n", titles[k]);
        printf("┌");
        for (int j = 0; j < width; j++) printf("─");
        printf("┐\n");
        
        for (int i = 0; i < height; i++) {
            printf("│\033[32m%s\033[0m│\n", canvas[k][i]);
        }
        
        printf("└");
        for (int j = 0; j < width; j++) printf("─");
        printf("┘\n");
        printf("  %-20s", "1.0");
        for (int j = 21; j < width - 6; j++) printf(" ");
        printf("-1.0\n");
    }
}

//Медианный фильтр
double* median_filter(double* signal, int N, int window_size, void (*sort_func)(double[], int, int)) {
    double* filtered = (double*)malloc(N * sizeof(double));
    if (filtered == NULL) {
        printf("Ошибка выделения памяти для filtered!\n");
        return NULL;
    }
    double* temp = (double*)malloc(window_size * sizeof(double));
    if (temp == NULL) {
        printf("Ошибка выделения памяти для temp!\n");
        free(filtered);
        return NULL;
    }
    int half = window_size / 2;
    for (int i = 0; i < N; i++) {
        int count = 0;
        for (int j = -half; j <= half; j++) {
            int index = i + j;
            if (index >= 0 && index < N) {
                temp[count++] = signal[index];
            }
        }
        if (count > 0) {
            sort_func(temp, 0, count - 1);
            filtered[i] = temp[count / 2];
        } else {
            filtered[i] = signal[i];
        }
    }
    free(temp);
    return filtered;
}

int main() {
    int N;
    do {
        printf("Введите размер массива (положительное целое число): ");
        if (scanf("%d", &N) != 1) {
            //Очистка буфера от некорректного ввода (буквы, символы)
            while (getchar() != '\n');
            printf("Ошибка: введите корректное целое число.\n");
            N = 0;  //Сброс, чтобы цикл продолжился
        }
        else if (N <= 0) {
            printf("Ошибка: размер должен быть положительным.\n");
        }
    } while (N <= 0);

    double *original = (double *)malloc(N * sizeof(double));
    double *noisy = (double *)malloc(N * sizeof(double));
    if (original == NULL || noisy == NULL) {
        printf("Ошибка выделения памяти!\n");
        printf("Перезапустите программу и попробуйте снова.\n");
        free(original);
        free(noisy);
        return 1;
    }

    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        double t = (double)i / N; //нормированное время
        original[i] = sin(2 * M_PI * t); //гармонический сигнал (синусоида)
        //добавляем шумовую компоненту: случайное значение от -0.1 до 0.1
        noisy[i] = original[i] + ((double)rand() / RAND_MAX - 0.5) * 0.2;
    }

    SortInfo sorts[] = {
        {"Пузырьковая сортировка", bubble_sort},
        {"Сортировка вставками", insertion_sort},
        {"Сортировка выбором", selection_sort},
        {"Быстрая сортировка", quicksort}
    };
    int num_sorts = sizeof(sorts) / sizeof(sorts[0]);

    for(int s = 0; s < num_sorts; s++) {
        double* filtered = median_filter(noisy, N, 5, sorts[s].sort_func);
        if (filtered == NULL) {
            free(original);
            free(noisy);
            return 1;
        }

        /*printf("\n#Фильтрация с использованием %s:\n", sorts[s].name);
        printf("_______________________________________________________\n");
        printf("| %-8s    | %-10s   | %-11s | %-12s |\n", "Время", "Оригинал", "Зашумлённый", "Фильтрованный");
        printf("|----------|------------|-------------|---------------|\n");
        for (int i = 0; i < N; i++) {
            double t = (double)i / (N > 1 ? N - 1 : 1); //нормированное время от 0 до 1
            printf("| %-8.4f | %-10.6f | %-11.6f | %-12.6f  |\n", t, original[i], noisy[i], filtered[i]);
        }
        printf("-------------------------------------------------------\n");
        */
        //Графический вывод функций
        plot_signals(original, noisy, filtered, N, sorts[s].name);
        
        free(filtered);
    }

    free(original);
    free(noisy);
    return 0;
}