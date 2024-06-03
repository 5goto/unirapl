#include <iostream>
#include <vector>
#include "../amd/interface.h"

void bubbleSort(std::vector<int>& arr) {
    int n = arr.size();
    bool swapped;

    do {
        swapped = false;
        for (int i = 1; i < n; i++) {
            if (arr[i - 1] > arr[i]) {
                std::swap(arr[i - 1], arr[i]);
                swapped = true;
            }
        }
    } while (swapped);
}

int main() {
    // Настройка параметров задачи
    int numElements = 500;  // Начальное количество элементов
    int maxElement = 100;     // Начальный максимальный элемент

    // Цикл для запуска задачи с повышенным временем выполнения
    for (int i = 0; i < 8; i++) {
        // Генерация набора данных
        std::vector<int> arr;
        srand(time(NULL));  // Инициализация генератора случайных чисел
        for (int j = 0; j < numElements; j++) {
            arr.push_back(rand() % maxElement + 1);
        }

        // Многократное увеличение количества элементов и максимального элемента
        numElements *= 2;
        maxElement *= 2;
        double energy;

        // Запуск алгоритма
        Rapl* h = begin_energy_measurement();
        clock_t start = clock();
        bubbleSort(arr);
        clock_t end = clock();
        energy = complete_energy_measurement(h);

        // Вывод результатов
        std::cout << "==============================" << std::endl;
        std::cout << "Итерация " << i + 1 << std::endl;
        std::cout << "Число элементов: " << numElements << std::endl;
        std::cout << "Максимальный элемент: " << maxElement << std::endl;
        std::cout << "Время выполнения (мс): " << (double)(end - start) / CLOCKS_PER_SEC * 1000 << std::endl;
        std::cout << "Энергия (J): " << energy << std::endl;
        std::cout << "==============================" << std::endl;
    }

    return 0;
}
