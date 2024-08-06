#ifndef POINCARE_POINT_2D_LAYOUT_NODE_H
#define POINCARE_POINT_2D_LAYOUT_NODE_H

#include "two_rows_layout.h"

namespace Poincare {

class Point2DLayoutNode final : public TwoRowsLayoutNode {
 public:
  using TwoRowsLayoutNode::TwoRowsLayoutNode;

  // Layout
  Type otype() const override { return Type::Point2DLayout; }

  // SerializableNode
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // PoolObject
  size_t size() const override { return sizeof(Point2DLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Point2DLayout";
  }
#endif
};

class Point2DLayout final
    : public LayoutTwoChildren<Point2DLayout, Point2DLayoutNode> {
 public:
  Point2DLayout() = delete;
};

}  // namespace Poincare

#endif
