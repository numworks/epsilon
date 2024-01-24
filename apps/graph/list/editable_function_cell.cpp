#include "editable_function_cell.h"

#include <escher/palette.h>

using namespace Escher;

namespace Graph {

EditableFunctionCell::EditableFunctionCell(
    Escher::Responder* parentResponder,
    Escher::LayoutFieldDelegate* layoutFieldDelegate,
    Escher::StackViewController* modelsStackController)
    : TemplatedFunctionCell<Shared::WithEditableExpressionCell>(),
      m_templateButtonState(
          State::Visible),  // ButtonCell is visible by default.
      m_buttonCell(expressionCell()->layoutField(),
                   Invocation::Builder<ViewController>(
                       [](ViewController* controller, void* sender) {
                         App::app()->displayModalViewController(
                             controller, 0.f, 0.f,
                             Metric::PopUpMarginsNoBottom);
                         return true;
                       },
                       modelsStackController)) {
  // Initialize expression cell
  expressionCell()->layoutField()->setParentResponder(parentResponder);
  expressionCell()->layoutField()->setDelegate(layoutFieldDelegate);
}

void EditableFunctionCell::setTemplateButtonState(State state) {
  if (m_templateButtonState == state) {
    return;
  }

  if (m_templateButtonState == State::Highlighted) {
    // Highlighted -> {Hidden, Visible}
    App::app()->setFirstResponder(expressionCell()->layoutField());
    m_buttonCell.setHighlighted(false);
  }

  if (state == State::Hidden) {
    // {Visible, Highlighted} -> Hidden
    m_buttonCell.setVisible(false);
    layoutSubviews();
  } else if (state == State::Highlighted) {
    // {Visible, Hidden} -> Highlighted
    m_buttonCell.setHighlighted(true);
    App::app()->setFirstResponder(&m_buttonCell);
  } else {
    // {Highlighted, Hidden} -> Visible
    assert(state == State::Visible);
    m_buttonCell.setVisible(true);
    layoutSubviews();
  }

  m_templateButtonState = state;
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
  if (m_buttonCell.isVisible()) {
    // Only draw the button if the expression is empty
    KDSize buttonSize = m_buttonCell.minimalSizeForOptimalDisplay();
    templateButtonRect =
        KDRect(bounds().width() - rightMargin - buttonSize.width() -
                   k_templateButtonMargin,
               (bounds().height() - buttonSize.height()) / 2, buttonSize);
  }
  setChildFrame(&m_buttonCell, templateButtonRect, force);
}

bool EditableFunctionCell::ButtonCell::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace) {
    return true;
  }

  return Escher::ButtonCell::handleEvent(event);
}

}  // namespace Graph
