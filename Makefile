backpack_pkg: examples/backpack_pkg.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -o backpack_pkg

backpack_core: examples/backpack_core.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -o backpack_core

bubble_sort_pkg: examples/bubble_sort_pkg.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -o bubble_sort_pkg

bubble_sort_core: examples/bubble_sort_core.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -o bubble_sort_core

integral_pkg: examples/integral_pkg.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -fopenmp -o integral_pkg

integral_core: examples/integral_core.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -fopenmp -o integral_core

selection_sort_pkg: examples/selection_sort_pkg.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -o selection_sort_pkg

selection_sort_core: examples/selection_sort_core.cpp amd/rapl-ryzen.cpp amd/realisation.cpp
	g++ -std=c++20 $^ -o selection_sort_core

##################################
backpack_pkg_intel: examples_intel/backpack_pkg.cpp intel/Rapl.cpp intel/realisation.cpp
	g++ -std=c++20 $^ -o backpack_pkg_intel

backpack_core_intel: examples_intel/backpack_core.cpp intel/Rapl.cpp intel/realisation.cpp
	g++ -std=c++20 $^ -o backpack_core_intel

bubble_sort_pkg_intel: examples_intel/bubble_sort_pkg.cpp intel/Rapl.cpp intel/realisation.cpp
	g++ -std=c++20 $^ -o bubble_sort_pkg_intel

bubble_sort_core_intel: examples_intel/bubble_sort_core.cpp intel/Rapl.cpp intel/realisation.cpp
	g++ -std=c++20 $^ -o bubble_sort_core_intel

integral_pkg_intel: examples_intel/integral_pkg.cpp intel/Rapl.cpp intel/realisation.cpp
	g++ -std=c++20 $^ -fopenmp -o integral_pkg_intel

integral_core_intel: examples_intel/integral_core.cpp intel/Rapl.cpp intel/realisation.cpp
	g++ -std=c++20 $^ -fopenmp -o integral_core_intel

selection_sort_pkg_intel: examples_intel/selection_sort_pkg.cpp intel/Rapl.cpp intel/realisation.cpp
	g++ -std=c++20 $^ -o selection_sort_pkg_intel

selection_sort_core_intel: examples_intel/selection_sort_core.cpp intel/Rapl.cpp intel/realisation.cpp
	g++ -std=c++20 $^ -o selection_sort_core_intel


amd: backpack_pkg backpack_core bubble_sort_pkg bubble_sort_core integral_pkg integral_core selection_sort_pkg selection_sort_core selection_sort_core 

intel: backpack_pkg_intel backpack_core_intel bubble_sort_pkg_intel bubble_sort_core_intel integral_pkg_intel integral_core_intel selection_sort_pkg_intel selection_sort_core_intel

# Цель для очистки выходных файлов
clean:
	rm -f backpack examples/*.o
