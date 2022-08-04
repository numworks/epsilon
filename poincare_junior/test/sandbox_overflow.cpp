#include "print.h"

using namespace Poincare;

void testOverflowTreeSandbox(TreeCache * cache, TreeSandbox * sandbox) {
  std::cout << "\n---------------- Store (1 + 2) * 3 * 4 in cache ----------------" << std::endl;

  Multiplication::PushNode(sandbox, 3);
  Addition::PushNode(sandbox, 2);
  Integer::PushNode(sandbox, 1);
  Integer::PushNode(sandbox, 2);
  Integer::PushNode(sandbox, 3);
  Integer::PushNode(sandbox, 4);

  int treeId = cache->storeLastTree();
  print();

  std::cout << "\n---------------- Fill cache with copies until cache is emptied and initial tree disappear" << std::endl;
  bool executed;
  do {
    executed = sandbox->execute(treeId, [](TypeTreeBlock *, TreeSandbox * sandbox) {});
    cache->storeLastTree();
    print();
  } while (executed);
}
