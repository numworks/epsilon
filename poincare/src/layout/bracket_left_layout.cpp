#include "bracket_left_layout.h"
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
  m_operandHeight = BracketLeftRightLayout::k_minimalOperandHeight;
  int currentNumberOfOpenBrackets = 1;
  int numberOfBrothers = m_parent->numberOfChildren();
  for (int i = m_parent->indexOfChild(this) + 1; i < numberOfBrothers; i++) {
    ExpressionLayout * brother = m_parent->editableChild(i);
    if (brother->isRightBracket()) {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        return;
      }
    } else if (brother->isLeftBracket()) {
      currentNumberOfOpenBrackets++;
    }
    KDCoordinate brotherHeight = brother->size().height();
    if (brotherHeight > m_operandHeight) {
      m_operandHeight = brotherHeight;
    }
  }
}

void BracketLeftLayout::computeBaseline() {
  assert(m_parent != nullptr);
  m_baseline = operandHeight()/2;
  int currentNumberOfOpenBrackets = 1;
  int numberOfBrothers = m_parent->numberOfChildren();
  for (int i = m_parent->indexOfChild(this) + 1; i < numberOfBrothers; i++) {
    ExpressionLayout * brother = m_parent->editableChild(i);
    if (brother->isRightBracket()) {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        break;
      }
    } else if (brother->isLeftBracket()) {
      currentNumberOfOpenBrackets++;
    }
    if (brother->baseline() > m_baseline) {
      m_baseline = brother->baseline();
    }
  }
  m_baselined = true;
}

}
