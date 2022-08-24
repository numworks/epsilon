#include "poincare_junior/handle.h"
#include "print.h"

void playWithConstexprNodes() {
  TreeCache * cache = TreeCache::sharedCache();
  TreeSandbox * sandbox = cache->sandbox();

  constexpr TreeNode value = 42_n;

  // TODO: adding constexpr below is broken
  TreeNode node = Addition::Of(1_n, value, 3_n);
  sandbox->pushTree(node);

  TypeTreeBlock * valueMod = sandbox->pushTree(Subtraction::Of(24_n, 12_n));
  valueMod->basicReduction();

  print();
}
