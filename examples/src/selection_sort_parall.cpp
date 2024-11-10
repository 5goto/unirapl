#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include <thread>
#include <future> 
#include "../../client/UniraplInterface/UniraplInterface.h"

// Модифицированный алгоритм сортировки выбором для параллельной обработки
void selectionSortParallel(std::vector<int>& arr) {
    int n = arr.size();
    int numThreads = std::thread::hardware_concurrency(); // Количество доступных ядер процессора
    int itemsPerThread = n / numThreads;

    std::vector<std::future<void>> results;
    for (int i = 0; i < numThreads; i++) {
        int start = i * itemsPerThread;
        int end = (i == numThreads - 1) ? n : start + itemsPerThread;

        // Запуск задачи в отдельном потоке
        results.push_back(std::async(std::launch::async, [&arr, start, end]() {
            for (int i = start; i < end - 1; i++) {
                int min_idx = i;
                for (int j = i + 1; j < end; j++) {
                    if (arr[j] < arr[min_idx]) {
                        min_idx = j;
                    }
                }
                std::swap(arr[i], arr[min_idx]);
            }
        }));
    }

    // Ожидание завершения всех потоков
    for (auto& result : results) {
        result.get();
    }

    // Дополнительная сортировка между частями
    for (int i = 1; i < numThreads; i++) {
        int start = i * itemsPerThread;
        for (int j = start; j < n; j++) {
            if (arr[j] < arr[start - 1]) {
                std::swap(arr[j], arr[start - 1]);
            }
        }
    }
}

int main() {
    int numElements = 5000;  // Начальное количество элементов
    int maxElement = 1000;     // Начальный максимальный элемент

    changeMode("PKG");

    for (int i = 0; i < 18; i++) {
        std::vector<int> arr;
        srand(time(NULL));  // Инициализация генератора случайных чисел
        for (int j = 0; j < numElements; j++) {
            arr.push_back(rand() % maxElement + 1);
        }

        numElements *= 1.1;
        maxElement *= 1.1;
        std::string energy;

        startMeasure();
        clock_t start = clock();
        selectionSortParallel(arr);
        clock_t end = clock();
        energy = endMeasure();

        std::cout << "==============================" << std::endl;
        std::cout << "Итерация " << i + 1 << std::endl;
        std::cout << "Число элементов: " << numElements << std::endl;
        std::cout << "Максимальный элемент: " << maxElement << std::endl;
        std::cout << "Время выполнения (мс): " << (double)(end - start) / CLOCKS_PER_SEC * 1000 << std::endl;
        std::cout << "Энергия(J): " << energy << std::endl;
        std::cout << "==============================" << std::endl;
    }

    return 0;
}
