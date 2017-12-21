#include "bracket_right_layout.h"
extern "C" {
#include <assert.h>
}

namespace Poincare {

ExpressionLayout * BracketRightLayout::clone() const {
  BracketRightLayout * layout = new BracketRightLayout();
  return layout;
}

void BracketRightLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->fillRect(KDRect(p.x()+k_widthMargin, p.y(), k_lineThickness, operandHeight()), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y(), k_bracketWidth, k_lineThickness), expressionColor);
  ctx->fillRect(KDRect(p.x()+k_widthMargin-k_bracketWidth+1, p.y() + operandHeight(), k_bracketWidth, k_lineThickness), expressionColor);
}

void BracketRightLayout::computeOperandHeight() {
  assert(m_parent != nullptr);
  m_operandHeight = BracketLeftRightLayout::k_minimalOperandHeight;
  int currentNumberOfOpenBrackets = 1;
  for (int i = m_parent->indexOfChild(this) - 1; i >= 0; i--) {
    ExpressionLayout * brother = m_parent->editableChild(i);
    if (brother->isLeftBracket()) {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        return;
      }
    } else if (brother->isRightBracket()) {
      currentNumberOfOpenBrackets++;
    }
    KDCoordinate brotherHeight = brother->size().height();
    if (brotherHeight > m_operandHeight) {
      m_operandHeight = brotherHeight;
    }
  }
}

void BracketRightLayout::computeBaseline() {
  assert(m_parent != nullptr);
  m_baseline = operandHeight()/2;
  int currentNumberOfOpenBrackets = 1;
  for (int i = m_parent->indexOfChild(this) - 1; i >= 0; i--) {
    ExpressionLayout * brother = m_parent->editableChild(i);
    if (brother->isLeftBracket()) {
      currentNumberOfOpenBrackets--;
      if (currentNumberOfOpenBrackets == 0) {
        break;
      }
    } else if (brother->isRightBracket()) {
      currentNumberOfOpenBrackets++;
    }
    if (brother->baseline() > m_baseline) {
      m_baseline = brother->baseline();
    }
  }
  m_baselined = true;
}

}
