#include "Rapl.h"
#include <iostream>
#include <fstream>
#include <ctime>


Rapl* begin_energy_measurement();
double complete_energy_measurement(Rapl* h);

Rapl* begin_energy_measurement_for_core(unsigned core_index);
double complete_energy_measurement_for_core(Rapl* h);


using namespace std;

struct CsvData {
    string name;
    ofstream file;
    
    CsvData(string name) {
        this->name = name;
        
        // Создать имя файла с уникальной меткой времени.
        time_t now = time(0);
        tm *ltm = localtime(&now);
        string timestamp = to_string(ltm->tm_year + 1900) + "-" +
                           to_string(ltm->tm_mon + 1) + "-" +
                           to_string(ltm->tm_mday) + "_" +
                           to_string(ltm->tm_hour) + "-" +
                           to_string(ltm->tm_min) + "-" +
                           to_string(ltm->tm_sec);
        string filename = name + "_" + timestamp + ".csv";
        
        file.open(filename);
        if (!file.is_open()) {
            cerr << "Ошибка при открытии файла: " << filename << endl;
            exit(1);
        }
    }
    
    void write(int a, double b) {
        file << a << "," << b << endl;
    }
    
    ~CsvData() {
        file.close();
    }
};