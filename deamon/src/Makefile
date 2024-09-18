DIR_BIN = ../bin

all: RaplCore/*.cpp RaplDeamon/*.cpp utils/*.cpp
	@if [ ! -d $(DIR_BIN) ]; then \
	mkdir $(DIR_BIN); \
	fi

	g++ -g -std=c++20 $^ -o ../bin/unirapl


# Цель для очистки выходных файлов
clean:
	rm -f backpack examples/*.o
