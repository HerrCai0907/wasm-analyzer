
#include "analyzer.hpp"
#include "parser.hpp"
#include <iostream>

using namespace wa;

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    std::cerr << "invalid arguments\n";
  }
  Parser parser{argv[1]};
  Analyzer analyzer{parser.parse()};

  analyzer.analyze();
}
