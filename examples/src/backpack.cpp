#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <algorithm>
#include "../../client/UniraplInterface/UniraplInterface.h"


// Реализация алгоритма жадной упаковки рюкзака
int greedyKnapsack(const std::vector<int>& weights, const std::vector<int>& values, int capacity) {
    // Сортировка предметов по соотношению ценности к весу
    std::vector<std::pair<double, int>> valuePerWeight;
    for (int i = 0; i < weights.size(); i++) {
        valuePerWeight.push_back(std::make_pair((double)values[i] / weights[i], i));
    }
    std::sort(valuePerWeight.begin(), valuePerWeight.end(), std::greater<>());

    // Инициализация рюкзака
    int currentWeight = 0;
    int totalValue = 0;
    std::vector<bool> itemsIncluded(weights.size(), false);

    // Итерация по предметам в порядке убывания соотношения ценности к весу
    for (int i = 0; i < valuePerWeight.size(); i++) {
        // Если предмет помещается в рюкзак, включаем его
        if (currentWeight + weights[valuePerWeight[i].second] <= capacity) {
            currentWeight += weights[valuePerWeight[i].second];
            totalValue += values[valuePerWeight[i].second];
            itemsIncluded[valuePerWeight[i].second] = true;
        }
    }

    return totalValue;
}

// Генерация набора весов и стоимостей предметов
void generateData(std::vector<int>& weights, std::vector<int>& values, int numItems, int maxValue, int maxWeight) {
    srand(time(NULL));  // Инициализация генератора случайных чисел

    for (int i = 0; i < numItems; i++) {
        weights.push_back(rand() % maxWeight + 1);
        values.push_back(rand() % maxValue + 1);
    }
}

int main() {
    // Настройка параметров задачи
    int numItems = 10000;  // Количество предметов
    int maxValue = 100;     // Максимальная стоимость предмета
    int maxWeight = 100;    // Максимальный вес предмета
    int capacity = 500;     // Вместимость рюкзака
    float freq_1, freq_2;

    changeMode("ALL_CORES");

    for (int i = 0; i < 18; i++) {  
        std::vector<int> weights;
        std::vector<int> values;
        generateData(weights, values, numItems, maxValue, maxWeight);

        numItems *= 1.1;
        maxValue *= 1.1;
        maxWeight *= 1.1;

        std::string energy;

        // Запуск алгоритма
        startMeasure();
        clock_t start = clock();

        int maxValuePacked = greedyKnapsack(weights, values, capacity);
        clock_t end = clock();
        energy = endMeasure();

        std::cout << "==============================" << std::endl;
        std::cout << "Итерация " << i + 1 << std::endl;
        std::cout << "Число предметов: " << numItems << std::endl;
        std::cout << "Максимальная стоимость предмета: " << maxValue << std::endl;
        std::cout << "Максимальный вес предмета: " << maxWeight << std::endl;
        std::cout << "Вместимость рюкзака: " << capacity << std::endl;
        std::cout << "Максимальная упакованная ценность: " << maxValuePacked << std::endl;
        std::cout << "Время выполнения (мс): " << (double)(end - start) / CLOCKS_PER_SEC * 1000 << std::endl;
        std::cout << "Энергия(J): " << energy << std::endl;
        std::cout << "==============================" << std::endl;
    }

    return 0;
}