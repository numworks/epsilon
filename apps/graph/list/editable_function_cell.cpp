#include "editable_function_cell.h"

#include <escher/palette.h>

using namespace Escher;

namespace Graph {

EditableFunctionCell::EditableFunctionCell(
    Escher::Responder* parentResponder,
    Escher::LayoutFieldDelegate* layoutFieldDelegate)
    : TemplatedFunctionCell<Shared::WithEditableExpressionCell>(),
      m_buttonCell(expressionCell()->layoutField(),
                   Invocation::Builder<void>([](void*, void*) { return true; },
                                             nullptr)) {
  // Initialize expression cell
  expressionCell()->layoutField()->setParentResponder(parentResponder);
  expressionCell()->layoutField()->setDelegate(layoutFieldDelegate);
}

View* EditableFunctionCell::subviewAtIndex(int index) {
  assert(index < numberOfSubviews());
  if (index == numberOfSubviews() - 1) {
    return &m_buttonCell;
  }
  return AbstractFunctionCell::subviewAtIndex(index);
}

void EditableFunctionCell::layoutSubviews(bool force) {
  setChildFrame(&m_ellipsisView,
                KDRect(bounds().width() - k_parametersColumnWidth, 0,
                       k_parametersColumnWidth, bounds().height()),
                force);
  KDCoordinate leftMargin = k_colorIndicatorThickness + k_expressionMargin;
  KDCoordinate rightMargin = k_expressionMargin + k_parametersColumnWidth;
  KDCoordinate availableWidth = bounds().width() - leftMargin - rightMargin;
  setChildFrame(expressionCell(),
                KDRect(leftMargin, 0, availableWidth, bounds().height()),
                force);
  setChildFrame(&m_messageTextView, KDRectZero, force);

  KDRect templateButtonRect = KDRectZero;
  if (isEmpty()) {
    // Only draw the button if the expression is empty
    KDSize buttonSize = m_buttonCell.minimalSizeForOptimalDisplay();
    templateButtonRect =
        KDRect(bounds().width() - rightMargin - buttonSize.width() -
                   k_templateButtonMargin,
               (bounds().height() - buttonSize.height()) / 2, buttonSize);
  }
  setChildFrame(&m_buttonCell, templateButtonRect, force);
}

// EditableFunctionCell::ButtonCell
bool EditableFunctionCell::ButtonCell::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    layoutField()->setEditing(true);
    return false;
  }
  if (event == Ion::Events::Up || event == Ion::Events::Down) {
    return true;
  } else if (event == Ion::Events::Left || event == Ion::Events::Backspace) {
    setHighlighted(false);
    layoutField()->setEditing(true);
    App::app()->setFirstResponder(layoutField());
    return true;
  }

  return Escher::ButtonCell::handleEvent(event);
}

}  // namespace Graph
