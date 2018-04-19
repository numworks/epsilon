#include "right_bracket_layout.h"
#include <escher/metric.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

ExpressionLayout * RightBracketLayout::clone() const {
  RightBracketLayout * layout = new RightBracketLayout();
  return layout;
}

void RightBracketLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_widthMargin, p.y(), k_lineThickness, operandHeight()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y() + operandHeight(), k_bracketWidth, k_lineThickness), expressionColor);
}

void RightBracketLayout::computeOperandHeight() {
  assert(m_parent != nullptr);
  m_operandHeight = Metric::MinimalBracketAndParenthesisHeight;
  int currentNumberOfOpenBrackets = 1;
  for (int i = m_parent->indexOfChild(this) - 1; i >= 0; i--) {
    ExpressionLayout * sibling = m_parent->editableChild(i);
    if (sibling->isLeftBracket()) {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        return;
      }
    } else if (sibling->isRightBracket()) {
      currentNumberOfOpenBrackets++;
    }
    KDCoordinate siblingHeight = sibling->size().height();
    if (siblingHeight > m_operandHeight) {
      m_operandHeight = siblingHeight;
    }
  }
}

void RightBracketLayout::computeBaseline() {
  assert(m_parent != nullptr);
  m_baseline = operandHeight()/2;
  int currentNumberOfOpenBrackets = 1;
  for (int i = m_parent->indexOfChild(this) - 1; i >= 0; i--) {
    ExpressionLayout * sibling = m_parent->editableChild(i);
    if (sibling->isLeftBracket()) {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        break;
      }
    } else if (sibling->isRightBracket()) {
      currentNumberOfOpenBrackets++;
    }
    if (sibling->baseline() > m_baseline) {
      m_baseline = sibling->baseline();
    }
  }
  m_baselined = true;
}

}
