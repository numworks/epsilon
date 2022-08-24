#include "poincare_junior/handle.h"
#include "print.h"

namespace Constants {

constexpr TreeNode e = u'π'_n;

}

void playWithConstexprNodes() {
  TreeCache * cache = TreeCache::sharedCache();
  TreeSandbox * sandbox = cache->sandbox();

  constexpr TreeNode value = 42_n;
  constexpr TreeNode π = u'π'_n;

  TreeNode node = Addition::Of(π, u'e'_n, Constants::e);
  sandbox->pushTree(node);

  TypeTreeBlock * valueMod = sandbox->pushTree(Subtraction::Of(value, 12_n));
  valueMod->basicReduction();

  print();
}
