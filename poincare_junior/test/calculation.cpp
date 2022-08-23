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
  Calculation(CachedTree cachedInput);
  TypeTreeBlock * input() { return m_input; }
  Poincare::CachedTree output() { return m_output; }
private:
  constexpr static int k_bufferSize = 128;
  TypeTreeBlock m_input[k_bufferSize];
  Poincare::CachedTree m_output;
};

Calculation::Calculation(CachedTree cachedTree) :
  m_output([]() { return true; })
{
  cachedTree.send(
      [](const TypeTreeBlock * tree, void * buffer) {
        TypeTreeBlock * inputBuffer = static_cast<TypeTreeBlock *>(buffer);
        memcpy(inputBuffer, tree, tree->treeSize());
      },
      m_input
    );
  m_output = CachedTree(
      [](TypeTreeBlock * tree) {
        tree->basicReduction();
        return true;
      },
      input()
    );
}

void printCachedTree(CachedTree cachedTree) {
  cachedTree.send(
      [](const TypeTreeBlock * tree, void * result) {
        tree->log(std::cout);
      },
      nullptr
    );
}

void playWithCachedTree() {
  CachedTree inputTree([]{
      std::cout << "\n---------------- Input (1-2)/3/4 ----------------" << std::endl;
      Division::PushNode();
      Division::PushNode();
      Subtraction::PushNode();
      Integer::PushNode(1);
      Integer::PushNode(2);
      Integer::PushNode(3);
      Integer::PushNode(4);
      return true;
    });
  Calculation calculation(inputTree);
  std::cout << "\n---------------- Output ----------------" << std::endl;
  printCachedTree(calculation.output());

  print();
}
