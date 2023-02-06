#include "print.h"
#include <poincare_junior/include/expression.h>

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
  Calculation(const char * textInput);
  TypeBlock * input() { return m_input; }
  PoincareJ::Expression output() { return m_output; }
private:
  constexpr static int k_bufferSize = 128;
  TypeBlock m_input[k_bufferSize];
  PoincareJ::Expression m_output;
};

Calculation::Calculation(const char * textInput) {
  Expression::ParseFromText(textInput).dumpAt(m_input);
  m_output = Expression::CreateBasicReduction(m_input);
}

void testCalculation() {
  Calculation calculation("(1-2)/3/4");
#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n---------------- Push Calculation (1-2)/3/4 ----------------" << std::endl;
  calculation.output().log();
#endif
}
QUIZ_CASE(pcj_calculation) { testCalculation(); }

// Check SharedPointer

Expression expressionViolatingLifetimeOfData() {
  char input[20] = "1+2";
  Expression e = Expression::ParseFromText(input);
  // Corrupt the data source
  input[0] = 'a';
  return e;
}

void testRunTimeCrashIllFormedExpression() {
  Expression e = expressionViolatingLifetimeOfData();
#if POINCARE_MEMORY_TREE_LOG
  e.log();
#endif
}

// This test is expected to fail
// QUIZ__CASE(pcj_mustfail) {
//   testRunTimeCrashIllFormedExpression();
// }
