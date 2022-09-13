#include "print.h"
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

// Dummy parse
CachedTree Parse(const char * textInput) {
  // textInput == (1-2)/3/4
  std::cout << "\n---------------- Input " << textInput << "----------------" << std::endl;
  return CachedTree([]{
      Division::PushNode();
      Division::PushNode();
      Subtraction::PushNode();
      Integer::PushNode(1);
      Integer::PushNode(2);
      Integer::PushNode(3);
      Integer::PushNode(4);
      return true;
    });
}


Calculation::Calculation(const char * textInput) {
  Parse(textInput).dumpAt(m_input);
  m_output = input()->createBasicReduction();
}

void playWithCachedTree() {
  Calculation calculation("(1-2)/3/4");
  std::cout << "\n---------------- Output ----------------" << std::endl;
  calculation.output().log();
  print();
}
