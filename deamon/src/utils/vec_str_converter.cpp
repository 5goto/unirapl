#include "vec_str_converter.h"

// Функция для преобразования вектора в строку
std::string vectorToString(const std::vector<double>& vec) {
  std::stringstream ss;
  for (size_t i = 0; i < vec.size(); ++i) {
    ss << vec[i];
    if (i != vec.size() - 1) {
      ss << ",";  // Разделитель для чисел
    }
  }
  return ss.str();
}

// Функция для преобразования строки в вектор
std::vector<double> stringToVector(const std::string& str) {
  std::vector<double> vec;
  std::string token;
  std::stringstream ss(str);
  while (std::getline(ss, token, ',')) {
    vec.push_back(std::stod(token));
  }
  return vec;
}

// int main() {
//   // Исходный вектор
//   std::vector<double> numbers = {1.2};

//   // Преобразование вектора в строку
//   std::string strNumbers = vectorToString(numbers);
//   std::cout << "Вектор в строке: " << strNumbers << std::endl;

//   // Преобразование строки в вектор
//   std::vector<double> restoredNumbers = stringToVector(strNumbers);
//   std::cout << "Восстановленный вектор: ";
//   for (double num : restoredNumbers) {
//     std::cout << num << " ";
//   }
//   std::cout << std::endl;

//   return 0;
// }
