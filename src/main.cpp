
#include "analyzer.hpp"
#include "parser.hpp"
#include <iostream>
#include <memory>

using namespace wa;

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    std::cerr << "invalid arguments\n";
  }
  Parser parser{argv[1]};
  AnalyzerManager analyzer{parser.parse()};
  auto context = std::make_shared<AnalyzerContext>();

  analyzer.registerAnalyzer(createPrinterAnalyzer(context));
  analyzer.registerAnalyzer(createCfgBuilderAnalyzer(context));

  analyzer.analyze();
}
