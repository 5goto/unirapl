import pandas as pd
import matplotlib.pyplot as plt
import os
from datetime import datetime

# Получаем список всех файлов в текущей директории с расширением .csv
csv_files = [f for f in os.listdir() if f.endswith('.csv')]

# Создаем словарь для хранения данных из каждого файла
data = {}

# Читаем данные из каждого файла и добавляем их в словарь
for filename in csv_files:
  df = pd.read_csv(filename, header=None, names=['Iteration', 'Energy'])
  data[filename[:-4]] = df

# Создаем график
fig, ax = plt.subplots(figsize=(16, 10)) # Устанавливаем размер фигуры

# Строим линии для каждого файла
for filename, df in data.items():
  ax.plot(df['Iteration'], df['Energy'], label=filename)

# Добавляем подписи осей и заголовок
ax.set_xlabel('Номер запуска алгоритма')
ax.set_ylabel('Энергопотребление (Дж)')
ax.set_title('График измерения энергопотребления по всем ядрам CPU')

# Добавляем легенду
ax.legend()

# Устанавливаем отметки на оси абсцисс каждую единицу
plt.xticks(df['Iteration'])

# Добавляем блок с текстом посередине сверху
fig.text(0.5, 0.75, "Процессор:AMD Ryzen 5 3600 6-Core Processor\nНачальный размер массива: 5000 элементов\nМаксимальный элемент массива: 1000 \nНа каждой итерации показатели увеличиваются на 10%", 
    ha="center", va="top", fontsize=10)

# Формируем имя файла с текущей меткой времени
timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
filename = f"energy_graph_{timestamp}.png"

# Сохраняем график в файл PNG с высоким качеством
plt.savefig(filename)

