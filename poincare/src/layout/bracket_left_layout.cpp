#include "bracket_left_layout.h"
#include <escher/metric.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

ExpressionLayout * BracketLeftLayout::clone() const {
  BracketLeftLayout * layout = new BracketLeftLayout();
  return layout;
}

void BracketLeftLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_lineThickness, operandHeight()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_externWidthMargin, p.y() + operandHeight(), k_bracketWidth, k_lineThickness), expressionColor);
}

void BracketLeftLayout::computeOperandHeight() {
  assert(m_parent != nullptr);
  m_operandHeight = Metric::MinimalBracketAndParenthesisHeight;
  int currentNumberOfOpenBrackets = 1;
  int numberOfSiblings = m_parent->numberOfChildren();
  for (int i = m_parent->indexOfChild(this) + 1; i < numberOfSiblings; i++) {
    ExpressionLayout * sibling = m_parent->editableChild(i);
    if (sibling->isRightBracket()) {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        return;
      }
    } else if (sibling->isLeftBracket()) {
      currentNumberOfOpenBrackets++;
    }
    KDCoordinate siblingHeight = sibling->size().height();
    if (siblingHeight > m_operandHeight) {
      m_operandHeight = siblingHeight;
    }
  }
}

void BracketLeftLayout::computeBaseline() {
  assert(m_parent != nullptr);
  m_baseline = operandHeight()/2;
  int currentNumberOfOpenBrackets = 1;
  int numberOfSiblings = m_parent->numberOfChildren();
  for (int i = m_parent->indexOfChild(this) + 1; i < numberOfSiblings; i++) {
    ExpressionLayout * sibling = m_parent->editableChild(i);
    if (sibling->isRightBracket()) {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        break;
      }
    } else if (sibling->isLeftBracket()) {
      currentNumberOfOpenBrackets++;
    }
    if (sibling->baseline() > m_baseline) {
      m_baseline = sibling->baseline();
    }
  }
  m_baselined = true;
}

}
