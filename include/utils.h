#pragma once
#include <string>
#include <sstream>
#include <iomanip>

// Парсит время формата "HH:MM" в минуты от 00:00
inline int parseTime(const std::string &s) {
  int h = std::stoi(s.substr(0,2));
  int m = std::stoi(s.substr(3,2));
  return h * 60 + m;
}

// Форматирует минуты от 00:00 обратно в "HH:MM"
inline std::string formatTime(int t) {
  int h = t / 60;
  int m = t % 60;
  std::ostringstream ss;
  ss << std::setw(2) << std::setfill('0') << h
     << ":" << std::setw(2) << std::setfill('0') << m;
  return ss.str();
}

// Округляет минуты вверх до целого числа часов
inline int ceilHours(int minutes) {
  return (minutes + 59) / 60;
}
