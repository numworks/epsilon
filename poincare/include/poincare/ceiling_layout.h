#ifndef POINCARE_CEILING_LAYOUT_NODE_H
#define POINCARE_CEILING_LAYOUT_NODE_H

#include <poincare/ceiling.h>
#include <poincare/bracket_pair_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class CeilingLayoutNode final : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;

  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Ceiling::s_functionHelper.name());
  }

  // TreeNode
  size_t size() const override { return sizeof(CeilingLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "CeilingLayout";
  }
#endif

protected:
  bool renderBottomBar() const override { return false; }
};

class CeilingLayout final : public Layout {
public:
  static CeilingLayout Builder(Layout l) { return CeilingLayout(l); }
private:
  explicit CeilingLayout(Layout l) : Layout(TreePool::sharedPool()->createTreeNode<CeilingLayoutNode>()) {
    replaceChildAtIndexInPlace(0, l);
  }
};

}

#endif
