#include "analyzer.hpp"
#include <iostream>
#include <memory>
#include <ostream>

namespace wa {

class Printer : public IAnalyzer {
public:
  Printer(std::shared_ptr<AnalyzerContext> const &context) : IAnalyzer(context) {}
  void analyze(Module &module) override;
};

void Printer::analyze(Module &module) {
  std::cout << "Module" << "\n";
  for (auto &function : module.m_functions) {
    std::cout << "  Function " << *function->get_type() << "\n";
    for (auto &instr : function->get_instr()) {
      std::cout << "    Instr: " << instr << "\n";
    }
  }
}

std::shared_ptr<IAnalyzer> createPrinterAnalyzer(std::shared_ptr<AnalyzerContext> context) {
  return std::make_shared<Printer>(context);
}

} // namespace wa
