#ifndef POINCARE_CURLY_BRACE_LAYOUT_NODE_H
#define POINCARE_CURLY_BRACE_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/brace_layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class CurlyBraceLayoutNode : public BraceLayoutNode {
public:
  // Dimensions
  constexpr static KDCoordinate k_curveHeight = 6;
  constexpr static KDCoordinate k_curveWidth = 5;
  constexpr static KDCoordinate k_centerHeight = 3;
  constexpr static KDCoordinate k_centerWidth = 3;
  constexpr static KDCoordinate k_barWidth = 1;
  // Margins
  constexpr static KDCoordinate k_horizontalExternalMargin = 1;
  constexpr static KDCoordinate k_horizontalInternalMargin = 1;
  constexpr static KDCoordinate k_verticalInternalMargin = 4;

  constexpr static KDCoordinate k_curlyBraceWidth = k_horizontalExternalMargin + (k_centerWidth + k_curveWidth - k_barWidth) + k_horizontalInternalMargin;

  using BraceLayoutNode::BraceLayoutNode;
  size_t size() const override { return sizeof(CurlyBraceLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "CurlyBraceLayout";
  }
#endif

protected:
  static KDCoordinate HeightGivenChildHeight(KDCoordinate childHeight) { return childHeight + k_verticalInternalMargin; }
  static KDCoordinate ChildHeightGivenLayoutHeight(KDCoordinate layoutHeight) { return layoutHeight - k_verticalInternalMargin; }
  static void RenderWithChildHeight(bool left, KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);

  KDSize computeSize() override { return KDSize(k_curlyBraceWidth, HeightGivenChildHeight(childHeight())); }
};

class LeftCurlyBraceLayoutNode final : public CurlyBraceLayoutNode {
public:
  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) { CurlyBraceLayoutNode::RenderWithChildHeight(true, childHeight, ctx, p, expressionColor, backgroundColor); }

  using CurlyBraceLayoutNode::CurlyBraceLayoutNode;
  Type type() const override { return Type::LeftCurlyBraceLayout; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "LeftCurlyBraceLayout";
  }
#endif
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { return SerializationHelper::CodePoint(buffer, bufferSize, '{'); }

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override { RenderWithChildHeight(CurlyBraceLayoutNode::ChildHeightGivenLayoutHeight(layoutSize().height()), ctx, p, expressionColor, backgroundColor); }
};

class LeftCurlyBraceLayout final : public LayoutNoChildren<LeftCurlyBraceLayout, LeftCurlyBraceLayoutNode> {
public:
  LeftCurlyBraceLayout() = delete;
};

class RightCurlyBraceLayoutNode final : public CurlyBraceLayoutNode {
public:
  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) { CurlyBraceLayoutNode::RenderWithChildHeight(false, childHeight, ctx, p, expressionColor, backgroundColor); }

  using CurlyBraceLayoutNode::CurlyBraceLayoutNode;
  Type type() const override { return Type::RightCurlyBraceLayout; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "RightCurlyBraceLayout";
  }
#endif
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { return SerializationHelper::CodePoint(buffer, bufferSize, '}'); }

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override { RenderWithChildHeight(CurlyBraceLayoutNode::ChildHeightGivenLayoutHeight(layoutSize().height()), ctx, p, expressionColor, backgroundColor); }
};

class RightCurlyBraceLayout final : public LayoutNoChildren<RightCurlyBraceLayout, RightCurlyBraceLayoutNode> {
public:
  RightCurlyBraceLayout() = delete;
};

}

#endif
