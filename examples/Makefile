DIR_BIN = bin

bubble: src/bubble_sort.cpp ../client/RaplSender/*.cpp ../client/UniraplInterface/*.cpp
	@if [ ! -d $(DIR_BIN) ]; then \
	mkdir $(DIR_BIN); \
	fi
	
	g++ -std=c++20 $^ -o bin/bubble


# Цель для очистки выходных файлов
clean:
	rm -f backpack examples/*.o
