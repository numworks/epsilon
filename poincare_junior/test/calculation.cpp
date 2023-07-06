#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/k_tree.h>

#include "helper.h"

using namespace PoincareJ;

// Dummy calculation class to simulate FileSystem or App::Snapshot

class Calculation {
  /* This is a simplified version of Calculation model:
   * - the m_inputBuffer simulated the model kept in the app snashot (to be able
   *   to redraw history when re-entering the application
   * - the output expression is cached but not persisted in snapshot.
   *
   * NB: in the real calculation models: inputLayout, outputLayout are persisted
   * and input/output are memoized.*/

 public:
  Calculation(const char* textInput);
  TypeBlock* input() { return m_buffer.blocks(); }
  Expression output() { return m_output; }

 private:
  constexpr static int k_bufferSize = 128;
  BlockBuffer<k_bufferSize> m_buffer;
  Expression m_output;
};

Calculation::Calculation(const char* textInput) {
  Expression::Parse(textInput).dumpAt(m_buffer.blocks());
  m_output = Expression::CreateSimplifyReduction(m_buffer.blocks());
}

QUIZ_CASE(pcj_calculation) {
  Calculation calculation("(1-2)/3/4");
#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n---------------- Push Calculation (1-2)/3/4 ----------------"
            << std::endl;
  calculation.output().log();
#endif
}

// Check BlockBuffer

QUIZ_CASE(pcj_calculation_type_block_buffer) {
  Calculation calculation("(1-2)/3/4");
  const Tree* input = Tree::FromBlocks(calculation.input());
  quiz_assert(input->type() == BlockType::Division);
}

// Check SharedPointer

Expression expressionViolatingLifetimeOfData() {
  char input[20] = "1+2";
  Expression e = Expression::Parse(input);
  // Corrupt the data source
  input[0] = 'a';
  return e;
}

// This test is expected to fail
// QUIZ__CASE(pcj_mustfail) {
//   Expression e = expressionViolatingLifetimeOfData();
// #if POINCARE_MEMORY_TREE_LOG
//   e.log();
// #endif
// }

QUIZ_CASE(pcj_poincare_and_back) {
  const Tree* exp = KAdd(KCos("ab"_e), 3_e);
  assert_trees_are_equal(exp, Expression::FromPoincareExpression(
                                  Expression::ToPoincareExpression(exp)));
}
