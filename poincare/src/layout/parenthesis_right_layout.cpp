#include "parenthesis_right_layout.h"
#include <escher/metric.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

const uint8_t topRightCurve[ParenthesisLeftRightLayout::k_parenthesisCurveHeight][ParenthesisLeftRightLayout::k_parenthesisCurveWidth] = {
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
};

const uint8_t bottomRightCurve[ParenthesisLeftRightLayout::k_parenthesisCurveHeight][ParenthesisLeftRightLayout::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xEE, 0x11},
  {0xFF, 0xFF, 0xFF, 0xBE, 0x45},
  {0xFF, 0xFF, 0xFF, 0x5A, 0xA9},
  {0xFF, 0xFF, 0xB6, 0x49, 0xFF},
  {0xFF, 0xBF, 0x40, 0xF2, 0xFF},
  {0x9A, 0x40, 0xEB, 0xFF, 0xFF},
  {0x66, 0xF9, 0xFF, 0xFF, 0xFF},
};

ExpressionLayout * ParenthesisRightLayout::clone() const {
  ParenthesisRightLayout * layout = new ParenthesisRightLayout();
  return layout;
}

bool ParenthesisRightLayout::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (*numberOfOpenParenthesis == 0 && !goingLeft) {
    return false;
  }
  *numberOfOpenParenthesis = goingLeft ? *numberOfOpenParenthesis + 1 : *numberOfOpenParenthesis - 1;
  return true;
}

void ParenthesisRightLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDRect frame = KDRect(p.x() + ParenthesisLeftRightLayout::k_widthMargin + ParenthesisLeftRightLayout::k_lineThickness - ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
      p.y() + ParenthesisLeftRightLayout::k_externHeightMargin,
      ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
      ParenthesisLeftRightLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topRightCurve, (KDColor *)(ParenthesisLeftRightLayout::s_parenthesisWorkingBuffer));

  frame = KDRect(p.x() + ParenthesisLeftRightLayout::k_widthMargin + ParenthesisLeftRightLayout::k_lineThickness - ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
    p.y() + operandHeight() - ParenthesisLeftRightLayout::k_parenthesisCurveHeight - ParenthesisLeftRightLayout::k_externHeightMargin,
    ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
    ParenthesisLeftRightLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomRightCurve, (KDColor *)(ParenthesisLeftRightLayout::s_parenthesisWorkingBuffer));

  ctx->fillRect(KDRect(p.x()+ParenthesisLeftRightLayout::k_widthMargin,
        p.y()+ParenthesisLeftRightLayout::k_parenthesisCurveHeight+2,
        ParenthesisLeftRightLayout::k_lineThickness,
        operandHeight() - 2*(ParenthesisLeftRightLayout::k_parenthesisCurveHeight+ParenthesisLeftRightLayout::k_externHeightMargin)),
      expressionColor);
}

void ParenthesisRightLayout::computeOperandHeight() {
  assert(m_parent != nullptr);
  m_operandHeight = Metric::MinimalBracketAndParenthesisHeight;
  KDCoordinate max_under_baseline = 0;
  KDCoordinate max_above_baseline = 0;
  int currentNumberOfOpenParentheses = 1;
  for (int i = m_parent->indexOfChild(this) - 1; i >= 0; i--) {
    ExpressionLayout * brother = m_parent->editableChild(i);
    if (brother->isLeftParenthesis()) {
      currentNumberOfOpenParentheses--;
      if (currentNumberOfOpenParentheses == 0) {
        if (max_under_baseline + max_above_baseline > m_operandHeight) {
          m_operandHeight = max_under_baseline + max_above_baseline;
        }
        return;
      }
    } else if (brother->isRightParenthesis()) {
      currentNumberOfOpenParentheses++;
    }
    KDCoordinate brotherHeight = brother->size().height();
    KDCoordinate brotherBaseline = brother->baseline();
    if (brotherHeight - brotherBaseline > max_under_baseline) {
      max_under_baseline = brotherHeight - brotherBaseline ;
    }
    if (brotherBaseline > max_above_baseline) {
      max_above_baseline = brotherBaseline;
    }
  }
  if (max_under_baseline + max_above_baseline > m_operandHeight) {
    m_operandHeight = max_under_baseline + max_above_baseline;
  }
}

void ParenthesisRightLayout::computeBaseline() {
  assert(m_parent != nullptr);
  int currentNumberOfOpenParentheses = 1;
  int indexInParent = m_parent->indexOfChild(this);
  if (indexInParent == 0) {
    // The parenthesis is the leftmost child of its parent.
    m_baseline = operandHeight()/2;
    m_baselined = true;
    return;
  }
  m_baseline = 0;
  for (int i = indexInParent - 1; i >= 0; i--) {
    ExpressionLayout * brother = m_parent->editableChild(i);
    if (brother->isLeftParenthesis()) {
      if (i == indexInParent - 1) {
        // If the parenthesis is immediately closed, we set the baseline to half
        // the parenthesis height.
        m_baseline = operandHeight()/2;
        break;
      }
      currentNumberOfOpenParentheses--;
      if (currentNumberOfOpenParentheses == 0) {
        break;
      }
    } else if (brother->isRightParenthesis()) {
      currentNumberOfOpenParentheses++;
    }
    if (brother->baseline() > m_baseline) {
      m_baseline = brother->baseline();
    }
  }
  m_baselined = true;
}

}







