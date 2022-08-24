#include "poincare_junior/handle.h"
#include "print.h"

template<unsigned L1, unsigned L2>
constexpr TreeNode<L1+L2+1> NewSubtraction(const TreeNode<L1> child1, const TreeNode<L2> child2) {
  return makeNary<false>(BlockType::Subtraction, child1, child2);
}

static constexpr TreeNode<5> NewInteger(uint8_t value) {
  return {IntegerBlock, ValueTreeBlock(1), ValueTreeBlock(value), ValueTreeBlock(1), IntegerBlock};
}

template<unsigned ...Len>
constexpr auto NewAddition(const TreeNode<Len> (&...children)) {
  return makeNary<true>(BlockType::Addition, children...);
}


void playWithConstexprNodes() {
  TreeCache * cache = TreeCache::sharedCache();
  TreeSandbox * sandbox = cache->sandbox();

  constexpr TreeNode value = NewInteger(42);

  // TODO: adding constexpr below is broken
  TreeNode node = NewAddition(NewInteger(1), value, NewInteger(3));
  sandbox->pushTree(node);

  TypeTreeBlock * valueMod = sandbox->pushTree(NewSubtraction(NewInteger(24), NewInteger(12)));
  valueMod->basicReduction();

  print();
}
