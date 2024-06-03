backpack_pkg: examples/backpack_pkg.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -o backpack_pkg

bubble_sort_pkg: examples/bubble_sort_pkg.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -o bubble_sort_pkg

integral_pkg: examples/integral_pkg.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -fopenmp -o integral_pkg

selection_sort_pkg: examples/selection_sort_pkg.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -o selection_sort_pkg

# Цель для очистки выходных файлов
clean:
	rm -f backpack examples/*.o
