#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/nth_root.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/square_root.h>

#include <algorithm>

namespace Poincare {

size_t NthRootLayoutNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  if (isSquareRoot()) {
    /* Case: squareRoot(x) or root(x,empty):
     * Write "SquareRootSymbol(radicandLayout) */
    return SerializationHelper::Prefix(
        this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
        SquareRoot::s_functionHelper.aliasesList().mainAlias(),
        SerializationHelper::ParenthesisType::System, 0);
  }
  // Case: root(x,n)
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      NthRoot::s_functionHelper.aliasesList().mainAlias(),
      SerializationHelper::ParenthesisType::System);
}

bool NthRootLayoutNode::protectedIsIdenticalTo(OLayout l) {
  assert(l.otype() == Type::NthRootLayout);
  NthRootLayout &nrl = static_cast<NthRootLayout &>(l);
  return hasUpperLeftIndex() == nrl.node()->hasUpperLeftIndex() &&
         LayoutNode::protectedIsIdenticalTo(l);
}

NthRootLayout NthRootLayout::Builder(OLayout child) {
  void *bufferNode = Pool::sharedPool->alloc(sizeof(NthRootLayoutNode));
  NthRootLayoutNode *node = new (bufferNode) NthRootLayoutNode(false);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, child);
  return static_cast<NthRootLayout &>(h);
}

NthRootLayout NthRootLayout::Builder(OLayout child, OLayout index) {
  void *bufferNode = Pool::sharedPool->alloc(sizeof(NthRootLayoutNode));
  NthRootLayoutNode *node = new (bufferNode) NthRootLayoutNode(true);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, child);
  h.replaceChildAtIndexInPlace(1, index);
  return static_cast<NthRootLayout &>(h);
}

}  // namespace Poincare
