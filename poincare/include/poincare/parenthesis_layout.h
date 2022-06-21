#ifndef POINCARE_PARENTHESIS_LAYOUT_NODE_H
#define POINCARE_PARENTHESIS_LAYOUT_NODE_H

#include <poincare/bracket_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <algorithm>

namespace Poincare {

class ParenthesisLayoutNode : public BracketLayoutNode {
  friend class SequenceLayoutNode;
public:
  // Dimensions
  constexpr static KDCoordinate k_curveWidth = 5;
  constexpr static KDCoordinate k_curveHeight = 7;
  // Margins
  constexpr static KDCoordinate k_widthMargin = 1;
  constexpr static KDCoordinate k_parenthesisWidth = k_widthMargin + k_curveWidth + k_widthMargin;

  using BracketLayoutNode::BracketLayoutNode;
  // TreeNode
  size_t size() const override { return sizeof(ParenthesisLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ParenthesisLayout";
  }
#endif

private:
  KDCoordinate width() const override { return k_parenthesisWidth; }
};

class LeftParenthesisLayoutNode final : public ParenthesisLayoutNode {
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;

  // Layout
  Type type() const override { return Type::LeftParenthesisLayout; }

  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);
  static KDPoint PositionGivenChildHeightAndBaseline(KDSize childSize, KDCoordinate childBaseline) {
    return BracketLayoutNode::PositionGivenChildHeightAndBaseline(true, k_parenthesisWidth, childSize, childBaseline);
  }

  // Serializable Node
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::CodePoint(buffer, bufferSize, '(');
  }

  // TreeNode
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "LeftParenthesisLayout";
  }
#endif

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
};

class LeftParenthesisLayout final : public LayoutNoChildren<LeftParenthesisLayout, LeftParenthesisLayoutNode> {
public:
  LeftParenthesisLayout() = delete;
};

class RightParenthesisLayoutNode final : public ParenthesisLayoutNode {
public:
  using ParenthesisLayoutNode::ParenthesisLayoutNode;

  // Layout
  Type type() const override { return Type::RightParenthesisLayout; }

  static void RenderWithChildHeight(KDCoordinate childHeight, KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor);
  static KDPoint PositionGivenChildHeightAndBaseline(KDSize childSize, KDCoordinate childBaseline) {
    return BracketLayoutNode::PositionGivenChildHeightAndBaseline(false, k_parenthesisWidth, childSize, childBaseline);
  }

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::CodePoint(buffer, bufferSize, ')');
  }

  // TreeNode
  size_t size() const override { return sizeof(RightParenthesisLayoutNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "RightParenthesisLayout";
  }
#endif

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override;
};

class RightParenthesisLayout final : public LayoutNoChildren<RightParenthesisLayout, RightParenthesisLayoutNode> {
public:
  RightParenthesisLayout() = delete;
};

}

#endif
