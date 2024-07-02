backpack: examples/backpack.cpp src/Rapl.cpp src/RaplConfig.cpp src/realisation.cpp
	g++ -std=c++20 $^ -o backpack

bubble_sort: examples/bubble_sort.cpp src/Rapl.cpp src/RaplConfig.cpp src/realisation.cpp
	g++ -std=c++20 $^ -o bubble_sort

integral: examples/integral.cpp src/Rapl.cpp src/RaplConfig.cpp src/realisation.cpp
	g++ -std=c++20 $^ -fopenmp -o integral

selection_sort: examples/selection_sort.cpp src/Rapl.cpp src/RaplConfig.cpp src/realisation.cpp
	g++ -std=c++20 $^ -o selection_sort_pkg

# Цель для очистки выходных файлов
clean:
	rm -f backpack examples/*.o
