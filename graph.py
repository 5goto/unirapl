import csv
import matplotlib.pyplot as plt
import os

# Получить список CSV-файлов в текущем каталоге
csv_files = [f for f in os.listdir('.') if f.endswith('.csv')]

# Итерироваться по каждому CSV-файлу
for csv_file in csv_files:
    # Открыть CSV-файл
    with open(csv_file, 'r') as f:
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
        plt.title('График энергопотребления для ' + csv_file)

        # Получить имя файла без расширения
        filename, ext = os.path.splitext(csv_file)

        # Сохранить график в виде изображения с тем же именем
        plt.savefig(filename + '.png')

        # Очистить график для следующего файла
        plt.clf()