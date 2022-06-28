#ifndef POINCARE_CURLY_BRACE_LAYOUT_NODE_H
#define POINCARE_CURLY_BRACE_LAYOUT_NODE_H

#include <poincare/layout_helper.h>
#include <poincare/bracket_layout.h>
#include <poincare/serialization_helper.h>
#include <algorithm>

namespace Poincare {

class CurlyBraceLayoutNode : public BracketLayoutNode {
public:
  // Dimensions
  constexpr static KDCoordinate k_curveHeight = 6;
  constexpr static KDCoordinate k_curveWidth = 5;
  constexpr static KDCoordinate k_centerHeight = 3;
  constexpr static KDCoordinate k_centerWidth = 3;
  // Margins
  constexpr static KDCoordinate k_horizontalExternalMargin = 1;
  constexpr static KDCoordinate k_horizontalInternalMargin = 1;
  constexpr static KDCoordinate k_curlyBraceWidth = k_horizontalExternalMargin + (k_centerWidth + k_curveWidth - k_lineThickness) + k_horizontalInternalMargin;

  using BracketLayoutNode::BracketLayoutNode;
  size_t size() const override { return sizeof(CurlyBraceLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "CurlyBraceLayout";
  }
#endif

protected:
  static void RenderWithChildHeight(bool left, KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);

private:
  KDCoordinate width() const override { return k_curlyBraceWidth; }
};

class LeftCurlyBraceLayoutNode final : public CurlyBraceLayoutNode {
public:
  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
    CurlyBraceLayoutNode::RenderWithChildHeight(true, childHeight, ctx, p, expressionColor, backgroundColor);
  }
  static KDPoint PositionGivenChildHeightAndBaseline(KDSize childSize, KDCoordinate childBaseline) {
    return BracketLayoutNode::PositionGivenChildHeightAndBaseline(true, k_curlyBraceWidth, childSize, childBaseline);
  }

  using CurlyBraceLayoutNode::CurlyBraceLayoutNode;
  Type type() const override { return Type::LeftCurlyBraceLayout; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "LeftCurlyBraceLayout";
  }
#endif
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { return SerializationHelper::CodePoint(buffer, bufferSize, '{'); }

private:
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override {
    RenderWithChildHeight(OptimalChildHeightGivenLayoutHeight(layoutSize(font).height()), ctx, p, expressionColor, backgroundColor);
  }
};

class LeftCurlyBraceLayout final : public LayoutNoChildren<LeftCurlyBraceLayout, LeftCurlyBraceLayoutNode> {
public:
  LeftCurlyBraceLayout() = delete;
};

class RightCurlyBraceLayoutNode final : public CurlyBraceLayoutNode {
public:
  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
    CurlyBraceLayoutNode::RenderWithChildHeight(false, childHeight, ctx, p, expressionColor, backgroundColor);
  }
  static KDPoint PositionGivenChildHeightAndBaseline(KDSize childSize, KDCoordinate childBaseline) {
    return BracketLayoutNode::PositionGivenChildHeightAndBaseline(false, k_curlyBraceWidth, childSize, childBaseline);
  }

  using CurlyBraceLayoutNode::CurlyBraceLayoutNode;
  Type type() const override { return Type::RightCurlyBraceLayout; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "RightCurlyBraceLayout";
  }
#endif
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { return SerializationHelper::CodePoint(buffer, bufferSize, '}'); }

private:
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override {
    RenderWithChildHeight(OptimalChildHeightGivenLayoutHeight(layoutSize(font).height()), ctx, p, expressionColor, backgroundColor);
  }
};

class RightCurlyBraceLayout final : public LayoutNoChildren<RightCurlyBraceLayout, RightCurlyBraceLayoutNode> {
public:
  RightCurlyBraceLayout() = delete;
};

}

#endif
