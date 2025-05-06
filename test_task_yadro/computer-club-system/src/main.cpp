#include "ClubSystem.h"
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return 1;
  }
  // Здесь числа столов, время и цена будут считаны из файла
  ClubSystem club(0, 0, 0, 0);
  club.processInputFile(argv[1]);
  return 0;
}
