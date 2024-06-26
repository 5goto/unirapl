// extern "C" {
// #include "interface.h"
// }
#include "../src/interface.h"
#include <omp.h>

double integral(auto f, double a, double b, unsigned long long n) {
	double dx = (b-a)/n;
	double result = 0.;
#pragma omp parallel
	{
		double my_result = 0;
		for (unsigned long long i = omp_get_thread_num(); i < n; i += omp_get_num_threads())
			my_result = f(a + dx * i) * dx;
#pragma omp critical
		{
			result += my_result;
		}
	}
	return result;
}

struct csv_table_row {
	double result;
	double energy;
};

auto run_experiment(auto load_fn) {
	csv_table_row r;
	//Rapl* h = begin_energy_measurement_for_core(0);
	Rapl* h = begin_energy_measurement();
	r.result = load_fn();
	r.energy = complete_energy_measurement(h);
	//r.energy = complete_energy_measurement_for_core(h, 0);
	return r;
}

#include <iostream>

int main(int argc, char** argv) {
	CsvData data("backpack");

	for (unsigned long long n = 1024; n <= 0x80000000; n+= n) {
		auto [r, e] = run_experiment([n] () {return integral([](double x) {return x * x;}, -1, 1, n);});
		std::cout << n << "." << r << "," << e << "\n";
		data.write(n, e);
	}
	return 0;
}