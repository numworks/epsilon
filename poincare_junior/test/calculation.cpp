#include "print.h"
#include "utils.h"
#include <poincare_junior/cached_tree.h>

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
  TypeTreeBlock * input() { return m_input; }
  Poincare::CachedTree output() { return m_output; }
private:
  constexpr static int k_bufferSize = 128;
  TypeTreeBlock m_input[k_bufferSize];
  Poincare::CachedTree m_output;
};

Calculation::Calculation(const char * textInput) {
  Parse(textInput).dumpAt(m_input);
  m_output = CachedTree::CreateBasicReduction(input());
}

void testCalculation() {
  Calculation calculation("(1-2)/3/4");
  std::cout << "\n---------------- Output ----------------" << std::endl;
  calculation.output().log();
  print();
}
