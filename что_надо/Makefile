.phony: clean
intel-test: rapl.obj ep.obj
	g++ $^ -pthread -fopenmp -g -o $@

rapl.obj:rapl.c
	gcc -c $< -g -o  $@
ep.obj:ep.cpp
	g++ -c $< --std=c++20 -fopenmp -g -o $@
clean:
	rm *.obj intel-test

