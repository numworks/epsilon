#ifndef POINCARE_UNREAL_H
#define POINCARE_UNREAL_H

#include <poincare/undefined.h>

namespace Poincare {

class UnrealNode final : public UndefinedNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(UnrealNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Unreal";
  }
#endif

  // Properties
  Type type() const override { return Type::Unreal; }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;
private:
};

class Unreal final : public Number {
public:
  Unreal() : Number(TreePool::sharedPool()->createTreeNode<UnrealNode>()) {}
  static const char * Name() {
    return "unreal";
  }
  static int NameSize() {
    return 7;
  }
};

}

#endif
