#ifndef POINCARE_SUM_LAYOUT_NODE_H
#define POINCARE_SUM_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/sequence_layout.h>

namespace Poincare {

class SumLayoutNode final : public SequenceLayoutNode {
public:
  using SequenceLayoutNode::SequenceLayoutNode;

  // Layout
  Type type() const override { return Type::SumLayout; }

  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  size_t size() const override { return sizeof(SumLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "SumLayout";
  }
#endif

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
};

class SumLayout final : public LayoutFourChildren<SumLayout, SumLayoutNode> {
public:
  using LayoutBuilder::Builder;
  SumLayout() = delete;
};

}

#endif
