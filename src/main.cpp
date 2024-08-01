
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
  AnalyzerManager analyzer_manager{parser.parse()};
  std::shared_ptr<AnalyzerContext> context{new AnalyzerContext(analyzer_manager)};

  analyzer_manager.register_analyzer(createPrinterAnalyzer(context));
  analyzer_manager.register_analyzer(createCfgBuilderAnalyzer(context));
  analyzer_manager.register_analyzer(createExtendCfgBuilderAnalyzer(context));

  analyzer_manager.analyze();
}
