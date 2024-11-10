
#include <iostream>
#include <vector>
#include "../../client/UniraplInterface/UniraplInterface.h"

void selectionSort(std::vector<int>& arr) {
    int n = arr.size();
    
    for(int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for(int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        std::swap(arr[i], arr[min_idx]);
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
        selectionSort(arr);
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
