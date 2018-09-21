#include "history_view_cell.h"
#include "app.h"
#include "../constant.h"
#include "selectable_table_view.h"
#include <assert.h>
#include <string.h>

namespace Calculation {

HistoryViewCell::HistoryViewCell(Responder * parentResponder) :
  Responder(parentResponder),
  m_calculation(),
  m_inputLayout(),
  m_exactOutputLayout(),
  m_approximateOutputLayout(),
  m_inputView(this),
  m_scrollableOutputView(this),
  m_selectedSubviewType(HistoryViewCell::SubviewType::Output)
{
}

Shared::ScrollableExactApproximateExpressionsView * HistoryViewCell::outputView() {
  return &m_scrollableOutputView;
}

void HistoryViewCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_inputView.setBackgroundColor(backgroundColor());
  m_scrollableOutputView.evenOddCell()->setEven(even);
}

void HistoryViewCell::setHighlighted(bool highlight) {
  m_highlighted = highlight;
  m_inputView.setBackgroundColor(backgroundColor());
  m_scrollableOutputView.evenOddCell()->setHighlighted(false);
  if (isHighlighted()) {
    if (m_selectedSubviewType == SubviewType::Input) {
      m_inputView.setBackgroundColor(Palette::Select);
    } else {
      m_scrollableOutputView.evenOddCell()->setHighlighted(true);
    }
  }
  reloadScroll();
}

Poincare::Layout HistoryViewCell::layout() const {
  if (m_selectedSubviewType == SubviewType::Input) {
    return m_inputLayout;
  } else {
    return m_scrollableOutputView.layout();
  }
}

void HistoryViewCell::reloadCell() {
  m_scrollableOutputView.evenOddCell()->reloadCell();
  layoutSubviews();
  reloadScroll();
}

void HistoryViewCell::reloadScroll() {
  m_inputView.reloadScroll();
  m_scrollableOutputView.reloadScroll();
}

KDColor HistoryViewCell::backgroundColor() const {
  KDColor background = m_even ? KDColorWhite : Palette::WallScreen;
  return background;
}


int HistoryViewCell::numberOfSubviews() const {
  return 2;
}

View * HistoryViewCell::subviewAtIndex(int index) {
  View * views[2] = {&m_inputView, &m_scrollableOutputView};
  return views[index];
}

void HistoryViewCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDSize inputSize = m_inputView.minimalSizeForOptimalDisplay();
  if (inputSize.width() + Metric::HistoryHorizontalMargin > width) {
    m_inputView.setFrame(KDRect(Metric::HistoryHorizontalMargin, k_digitVerticalMargin, width - Metric::HistoryHorizontalMargin, inputSize.height()));
  } else {
    m_inputView.setFrame(KDRect(Metric::HistoryHorizontalMargin, k_digitVerticalMargin, inputSize.width(), inputSize.height()));
  }
  KDSize outputSize = m_scrollableOutputView.minimalSizeForOptimalDisplay();
  if (outputSize.width() + Metric::HistoryHorizontalMargin > width) {
    m_scrollableOutputView.setFrame(KDRect(Metric::HistoryHorizontalMargin, inputSize.height() + 2*k_digitVerticalMargin, width - Metric::HistoryHorizontalMargin, height - inputSize.height() - 3*k_digitVerticalMargin));
  } else {
    m_scrollableOutputView.setFrame(KDRect(width - outputSize.width() - Metric::HistoryHorizontalMargin, inputSize.height() + 2*k_digitVerticalMargin, outputSize.width(), height - inputSize.height() - 3*k_digitVerticalMargin));
  }
}

void HistoryViewCell::setCalculation(Calculation * calculation) {
  if (*calculation == m_calculation) {
    return;
  }
  m_calculation = *calculation;
  m_inputLayout = calculation->createInputLayout();
  m_inputView.setLayout(m_inputLayout);
  App * calculationApp = (App *)app();
  /* Both output expressions have to be updated at the same time. Otherwise,
   * when updating one layout, if the second one still points to a deleted
   * layout, calling to layoutSubviews() would fail. */
  if (!m_exactOutputLayout.isUninitialized()) {
    m_exactOutputLayout = Poincare::Layout();
  }
  if (!calculation->shouldOnlyDisplayApproximateOutput(calculationApp->localContext())) {
    m_exactOutputLayout = calculation->createExactOutputLayout(calculationApp->localContext());
  }
  m_approximateOutputLayout = calculation->createApproximateOutputLayout(calculationApp->localContext());
  m_scrollableOutputView.setLayouts(m_approximateOutputLayout, m_exactOutputLayout);
  I18n::Message equalMessage = calculation->exactAndApproximateDisplayedOutputsAreEqual(calculationApp->localContext()) == Calculation::EqualSign::Equal ? I18n::Message::Equal : I18n::Message::AlmostEqual;
  m_scrollableOutputView.setEqualMessage(equalMessage);
}

void HistoryViewCell::didBecomeFirstResponder() {
  if (m_selectedSubviewType == SubviewType::Input) {
    app()->setFirstResponder(&m_inputView);
  } else {
    app()->setFirstResponder(&m_scrollableOutputView);
  }
}

HistoryViewCell::SubviewType HistoryViewCell::selectedSubviewType() {
  return m_selectedSubviewType;
}

void HistoryViewCell::setSelectedSubviewType(HistoryViewCell::SubviewType subviewType) {
  m_selectedSubviewType = subviewType;
  setHighlighted(isHighlighted());
}

bool HistoryViewCell::handleEvent(Ion::Events::Event event) {
  if ((event == Ion::Events::Down && m_selectedSubviewType == SubviewType::Input) ||
    (event == Ion::Events::Up && m_selectedSubviewType == SubviewType::Output)) {
    SubviewType otherSubviewType = m_selectedSubviewType == SubviewType::Input ? SubviewType::Output : SubviewType::Input;
    CalculationSelectableTableView * tableView = (CalculationSelectableTableView *)parentResponder();
    tableView->scrollToSubviewOfTypeOfCellAtLocation(otherSubviewType, tableView->selectedColumn(), tableView->selectedRow());
    HistoryViewCell * selectedCell = (HistoryViewCell *)(tableView->selectedCell());
    selectedCell->setSelectedSubviewType(otherSubviewType);
    app()->setFirstResponder(selectedCell);
    return true;
  }
  return false;
}

}
