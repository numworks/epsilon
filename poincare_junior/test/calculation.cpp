#include "print.h"
#include <poincare_junior/expression_reference.h>

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
  Poincare::ExpressionReference output() { return m_output; }
private:
  constexpr static int k_bufferSize = 128;
  TypeBlock m_input[k_bufferSize];
  Poincare::ExpressionReference m_output;
};

Calculation::Calculation(const char * textInput) {
  ExpressionReference::Parse(textInput).dumpAt(m_input);
  m_output = ExpressionReference::CreateBasicReduction(m_input);
}

void testCalculation() {
  Calculation calculation("(1-2)/3/4");
  std::cout << "\n---------------- Push Calculation (1-2)/3/4 ----------------" << std::endl;
  calculation.output().log();
  print();
}

// Check SharedPointer

ExpressionReference expressionViolatingLifetimeOfData() {
  char input[20] = "1+2";
  ExpressionReference e = ExpressionReference::Parse(input);
  // Corrupt the data source
  input[0] = 'a';
  return e;
}

void testRunTimeCrashIllFormedExpression() {
  ExpressionReference e = expressionViolatingLifetimeOfData();
  e.log();
}
