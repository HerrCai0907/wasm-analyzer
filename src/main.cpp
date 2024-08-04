
#include "analyzer.hpp"
#include "args.hpp"
#include "high_frequency_sub_expr.hpp"
#include "parser.hpp"

using namespace wa;

int main(int argc, char const *argv[]) {
  std::string wasm_file{};
  Args::get_arg_parser().add_argument("wasm file").store_into(wasm_file).required();

  Args::get_arg_parser().parse_args(argc, argv);

  Parser parser{wasm_file.c_str()};

  AnalyzerManager analyzer_manager{parser.parse()};

  analyzer_manager.analyze();

  if (AnalyzerManager::is_HighFrequencySubExpr_active()) {
    analyzer_manager.get_analyzer<HighFrequencySubExpr>()->dump_result();
  }
}
