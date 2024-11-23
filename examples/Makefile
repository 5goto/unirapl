DIR_BIN = bin

bubble: src/bubble_sort.cpp ../client/RaplSender/*.cpp ../client/UniraplInterface/*.cpp
	@if [ ! -d $(DIR_BIN) ]; then \
	mkdir $(DIR_BIN); \
	fi
	
	g++ -std=c++20 $^ -o bin/bubble

selection: src/selection_sort.cpp ../client/RaplSender/*.cpp ../client/UniraplInterface/*.cpp
	@if [ ! -d $(DIR_BIN) ]; then \
	mkdir $(DIR_BIN); \
	fi
	
	g++ -std=c++20 $^ -o bin/selection

backpack: src/backpack.cpp ../client/RaplSender/*.cpp ../client/UniraplInterface/*.cpp
	@if [ ! -d $(DIR_BIN) ]; then \
	mkdir $(DIR_BIN); \
	fi
	
	g++ -std=c++20 $^ -o bin/backpack

bubble_parall: src/bubble_sort_parall.cpp ../client/RaplSender/*.cpp ../client/UniraplInterface/*.cpp
	@if [ ! -d $(DIR_BIN) ]; then \
	mkdir $(DIR_BIN); \
	fi
	
	g++ -std=c++20 $^ -o bin/bubble_parall

selection_parall: src/selection_sort_parall.cpp ../client/RaplSender/*.cpp ../client/UniraplInterface/*.cpp
	@if [ ! -d $(DIR_BIN) ]; then \
	mkdir $(DIR_BIN); \
	fi
	
	g++ -std=c++20 $^ -o bin/selection_parall

backpack_parall: src/backpack_parall.cpp ../client/RaplSender/*.cpp ../client/UniraplInterface/*.cpp
	@if [ ! -d $(DIR_BIN) ]; then \
	mkdir $(DIR_BIN); \
	fi
	
	g++ -std=c++20 $^ -o bin/backpack_parall

all: bubble selection backpack bubble_parall selection_parall backpack_parall

# Цель для очистки выходных файлов
clean:
	rm -f backpack examples/*.o
