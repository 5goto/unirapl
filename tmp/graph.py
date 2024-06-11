import csv
import matplotlib.pyplot as plt

# Открыть CSV файл
with open('data.csv', 'r') as f:
    # Создать объект CSV-ридера
    reader = csv.reader(f)

    # Инициализировать списки для данных
    x = []
    y = []

    # Прочитать строки из CSV файла
    for row in reader:
        # Добавить данные в соответствующие списки
        x.append(int(row[0]))
        y.append(float(row[1]))

# Построить график
plt.plot(x, y)
plt.xlabel('Итерация')
plt.ylabel('Энергопотребление [J]')
plt.title('График энергопотребления')

# Показать график
plt.show()
