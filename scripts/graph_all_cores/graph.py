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
  df = pd.read_csv(filename, header=None, names=['Iteration', 'Core1', 'Core2', 'Core3', 'Core4', 'Core5', 'Core6'])
  data[filename[:-4]] = df

# Создаем график
fig, ax = plt.subplots(figsize=(16, 10))

# Цвета для ядер
core_colors = ['blue', 'green', 'red', 'purple', 'brown', 'orange']

# Строим линии для каждого файла, группируя по типу
for filename in data:
  # Получаем стиль линии, основываясь на типе файла
  linestyle = '-' # Сплошная линия для обычных файлов
  if filename.endswith('_parall'):
    linestyle = '--' # Пунктирная линия для файлов с _parall

  # Строим графики для каждого ядра
  for core in range(1, 7):
    ax.plot(data[filename]['Iteration'], data[filename][f'Core{core}'], 
        label=f"{filename} - Core{core}", color=core_colors[core-1], linestyle=linestyle)

# Добавляем подписи осей и заголовок
ax.set_xlabel('Номер запуска алгоритма')
ax.set_ylabel('Энергопотребление (Дж)')
ax.set_title('График измерения энергопотребления в режиме PKG. Сравнение парраллельных и последовательных алгоритмов')

# Добавляем легенду
ax.legend()

# Устанавливаем отметки на оси абсцисс каждую единицу
plt.xticks(data[list(data.keys())[0]]['Iteration'])

# Добавляем блок с текстом посередине сверху
fig.text(0.5, 0.75, "Процессор:AMD Ryzen 5 3600 6-Core Processor\nНачальный размер массива: 5000 элементов\nМаксимальный элемент массива: 1000 \nНа каждой итерации показатели увеличиваются на 10%", 
    ha="center", va="top", fontsize=10)

# Формируем имя файла с текущей меткой времени
timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
filename = f"energy_graph_{timestamp}.png"

# Сохраняем график в файл PNG с заданным разрешением
plt.savefig(filename)