#include "editable_function_cell.h"
#include <escher/palette.h>

using namespace Escher;

namespace Graph {

View * EditableFunctionCell::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_expressionField;
    default:
      assert(index == 1);
      return &m_ellipsisView;
  }
}

void EditableFunctionCell::layoutSubviews(bool force) {
  m_ellipsisView.setFrame(KDRect(bounds().width() - k_parametersColumnWidth, 0,
                                 k_parametersColumnWidth, bounds().height()),
                          force);
  KDCoordinate leftMargin = k_colorIndicatorThickness + k_margin;
  KDCoordinate rightMargin = k_margin + k_parametersColumnWidth;
  KDCoordinate availableWidth = bounds().width() - leftMargin - rightMargin;
  m_expressionField.setFrame(
    KDRect(leftMargin, 0, availableWidth, bounds().height()), force);
  m_messageTextView.setFrame(KDRectZero, force);
}

KDSize EditableFunctionCell::minimalSizeForOptimalDisplay() const {
  KDCoordinate minimalHeight =
      m_expressionField.minimalSizeForOptimalDisplay().height();
  KDCoordinate parameterHeight =
      m_ellipsisView.minimalSizeForOptimalDisplay().height();
  if (parameterHeight > minimalHeight) {
    // Leave enough height for parameter's menu elipsis.
    minimalHeight = parameterHeight;
  }
  return KDSize(bounds().width(), minimalHeight);
}

}
