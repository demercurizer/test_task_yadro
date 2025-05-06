#include "ClubSystem.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file>\n";
    return 1;
  }

  std::string inputFile = argv[1];
  ClubSystem club{/*tables*/ 0, /*open*/ 0, /*close*/ 0, /*price*/ 0};
  club.processInputFile(inputFile);
  return 0;
}
