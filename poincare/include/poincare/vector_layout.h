#ifndef POINCARE_VECTOR_LAYOUT_NODE_H
#define POINCARE_VECTOR_LAYOUT_NODE_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class VectorLayoutNode final : public LayoutNode {
public:
  // Layout
  Type type() const override { return Type::VectorLayout; }

  // SerializationHelperInterface
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "vector", true, 0);
  }

  virtual void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false);
  virtual void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false);

  // TreeNode
  size_t size() const override { return sizeof(VectorLayoutNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "VectorLayout";
  }
#endif

  constexpr static KDCoordinate k_arrowWidth = 5;
  constexpr static KDCoordinate k_arrowHeight = 9; 
protected:
  virtual KDSize computeSize();
  virtual KDCoordinate computeBaseline();
  virtual KDPoint positionOfChild(LayoutNode * child);
private:
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed);
  constexpr static KDCoordinate k_sideMargin = 2;
  constexpr static KDCoordinate k_topMargin = 1;
  constexpr static KDCoordinate k_arrowLineHeight = 1; // k_arrowHeight - k_arrowLineHeight must be even
};

class VectorLayout final : public Layout {
public:
  static VectorLayout Builder(Layout child) { return TreeHandle::FixedArityBuilder<VectorLayout, VectorLayoutNode>({child}); }
  VectorLayout() = delete;
};

}

#endif
