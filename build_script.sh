#!/bin/bash

# Получаем информацию о процессоре из файла /proc/cpuinfo
cpu_info=$(cat /proc/cpuinfo)

# Определяем тип процессора
if [[ "$cpu_info" =~ "vendor_id" ]]; then
    vendor=$(grep "vendor_id" /proc/cpuinfo | cut -d ":" -f 2 | tr -d " ")
    if [[ "$vendor" =~ "GenuineIntel" ]]; then
        echo "Процессор Intel";
        echo "Сборка...";
        cd intel;
        g++ -std=c++11 -g Rapl.cpp PowerMonitor.cpp -o monitor;
        echo "Завершено"; 
        cd ..;
	mv intel/monitor ./monitor;
    elif [[ "$vendor" =~ "AuthenticAMD" ]]; then
        echo "Процессор AMD";
        echo "Сборка...";
        cd amd;
        g++ rapl-ryzen.cpp PowerMonitor.cpp -o monitor; 
        echo "Завершено";
        cd ..;
	mv amd/monitor ./monitor;
    else
        echo "Не удалось определить тип процессора."
    fi
elif [[ "$cpu_info" =~ "model name" ]]; then
    model=$(grep "model name" /proc/cpuinfo | cut -d ":" -f 2 | tr -d " ")
    if [[ "$model" =~ "Intel" ]]; then
        echo "Процессор Intel";
	echo "Сборка...";
	cd intel;
	g++ -std=c++11 -g Rapl.cpp PowerMonitor.cpp -o monitor;
	echo "Завершено";
	cd ..;
	mv intel/monitor ./monitor;
    elif [[ "$model" =~ "AMD" ]]; then
        echo "Процессор AMD";
	echo "Сборка...";
	cd amd;
	g++ rapl-ryzen.cpp PowerMonitor.cpp -o monitor; 
	echo "Завершено";
	cd ..;
	mv amd/monitor ./monitor;
    else
        echo "Не удалось определить тип процессора."
    fi
else
    echo "Не удалось определить тип процессора."
fi
