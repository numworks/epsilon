#include "print.h"
#include <poincare_junior/include/expression.h>

using namespace Poincare;

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
  Poincare::Expression output() { return m_output; }
private:
  constexpr static int k_bufferSize = 128;
  TypeBlock m_input[k_bufferSize];
  Poincare::Expression m_output;
};

Calculation::Calculation(const char * textInput) {
  Expression::Parse(textInput).dumpAt(m_input);
  m_output = Expression::CreateBasicReduction(m_input);
}

void testCalculation() {
  Calculation calculation("(1-2)/3/4");
  std::cout << "\n---------------- Push Calculation (1-2)/3/4 ----------------" << std::endl;
  calculation.output().log();
  print();
}

// Check SharedPointer

Expression expressionViolatingLifetimeOfData() {
  char input[20] = "1+2";
  Expression e = Expression::Parse(input);
  // Corrupt the data source
  input[0] = 'a';
  return e;
}

void testRunTimeCrashIllFormedExpression() {
  Expression e = expressionViolatingLifetimeOfData();
  e.log();
}
