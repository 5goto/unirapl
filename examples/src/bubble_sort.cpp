#include <iostream>
#include <vector>
#include "../../client/UniraplInterface/UniraplInterface.h"

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
    int numElements = 5000;  // Начальное количество элементов
    int maxElement = 1000;     // Начальный максимальный элемент

    changeMode("ALL_CORES");

    for (int i = 0; i < 18; i++) {
        // Генерация набора данных
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
        bubbleSort(arr);
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
