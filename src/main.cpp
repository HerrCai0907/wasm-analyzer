
#include "analyzer.hpp"
#include "high_frequency_sub_expr.hpp"
#include "parser.hpp"
#include <exception>
#include <iostream>

using namespace wa;

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::cerr << "invalid arguments\n";
    std::terminate();
  }

  Parser parser{argv[1]};

  AnalyzerOption options{};
  const char *tmp = nullptr;
  for (int i = 2; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (tmp != nullptr) {
        options.set(tmp, true);
      }
      tmp = argv[i];
    } else {
      if (tmp != nullptr) {
        options.set(tmp, argv[i]);
      } else {
        std::cerr << "invalid arguments\n";
        std::terminate();
      }
    }
  }
  if (tmp != nullptr) {
    options.set(tmp, true);
  }

  AnalyzerManager analyzer_manager{options, parser.parse()};

  analyzer_manager.active_analyzer<HighFrequencySubExpr>();

  analyzer_manager.analyze();

  analyzer_manager.get_analyzer<HighFrequencySubExpr>()->dump_result();
}
