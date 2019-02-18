#include "history_view_cell.h"
#include "app.h"
#include "../constant.h"
#include "selectable_table_view.h"
#include <assert.h>
#include <string.h>

namespace Calculation {

/* HistoryViewCellDataSource */

HistoryViewCellDataSource::HistoryViewCellDataSource() :
  m_selectedSubviewType(SubviewType::Output) {}

void HistoryViewCellDataSource::setSelectedSubviewType(SubviewType subviewType, HistoryViewCell * cell) {
  m_selectedSubviewType = subviewType;
  if (cell) {
    cell->setHighlighted(cell->isHighlighted());
  }
}

/* HistoryViewCell */

HistoryViewCell::HistoryViewCell(Responder * parentResponder) :
  Responder(parentResponder),
  m_calculation(),
  m_inputLayout(),
  m_leftOutputLayout(),
  m_rightOutputLayout(),
  m_inputView(this),
  m_scrollableOutputView(this)
{
}

Shared::ScrollableExactApproximateExpressionsView * HistoryViewCell::outputView() {
  return &m_scrollableOutputView;
}

void HistoryViewCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_inputView.setBackgroundColor(backgroundColor());
  m_scrollableOutputView.setBackgroundColor(backgroundColor());
  m_scrollableOutputView.evenOddCell()->setEven(even);
}

void HistoryViewCell::setHighlighted(bool highlight) {
  assert(m_dataSource);
  m_highlighted = highlight;
  m_inputView.setExpressionBackgroundColor(backgroundColor());
  m_scrollableOutputView.evenOddCell()->setHighlighted(false);
  if (isHighlighted()) {
    if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
      m_inputView.setExpressionBackgroundColor(Palette::Select);
    } else {
      m_scrollableOutputView.evenOddCell()->setHighlighted(true);
    }
  }
  reloadScroll();
}

Poincare::Layout HistoryViewCell::layout() const {
  assert(m_dataSource);
  if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
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
  KDCoordinate maxFrameWidth = bounds().width();
  KDSize inputSize = m_inputView.minimalSizeForOptimalDisplay();
  m_inputView.setFrame(KDRect(
    0,
    0,
    min(maxFrameWidth, inputSize.width()),
    inputSize.height()
  ));
  KDSize outputSize = m_scrollableOutputView.minimalSizeForOptimalDisplay();
  m_scrollableOutputView.setFrame(KDRect(
    max(0, maxFrameWidth - outputSize.width()),
    inputSize.height(),
    min(maxFrameWidth, outputSize.width()),
    bounds().height() - inputSize.height()
  ));
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
  if (!m_leftOutputLayout.isUninitialized()) {
    m_leftOutputLayout = Poincare::Layout();
  }
  if (!m_rightOutputLayout.isUninitialized()) {
    m_rightOutputLayout = Poincare::Layout();
  }
  if (calculation->shouldOnlyDisplayExactOutput()) {
    m_rightOutputLayout = calculation->createExactOutputLayout();
  } else {
    m_rightOutputLayout = calculation->createApproximateOutputLayout(calculationApp->localContext());
    if (!calculation->shouldOnlyDisplayApproximateOutput(calculationApp->localContext())) {
      m_leftOutputLayout = calculation->createExactOutputLayout();
    }
  }
  m_scrollableOutputView.setLayouts(m_rightOutputLayout, m_leftOutputLayout);
  I18n::Message equalMessage = calculation->exactAndApproximateDisplayedOutputsAreEqual(calculationApp->localContext()) == Calculation::EqualSign::Equal ? I18n::Message::Equal : I18n::Message::AlmostEqual;
  m_scrollableOutputView.setEqualMessage(equalMessage);
}

void HistoryViewCell::didBecomeFirstResponder() {
  assert(m_dataSource);
  if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
    app()->setFirstResponder(&m_inputView);
  } else {
    app()->setFirstResponder(&m_scrollableOutputView);
  }
}

bool HistoryViewCell::handleEvent(Ion::Events::Event event) {
  assert(m_dataSource);
  if ((event == Ion::Events::Down && m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) ||
    (event == Ion::Events::Up && m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Output)) {
    HistoryViewCellDataSource::SubviewType otherSubviewType = m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input ? HistoryViewCellDataSource::SubviewType::Output : HistoryViewCellDataSource::SubviewType::Input;
    CalculationSelectableTableView * tableView = (CalculationSelectableTableView *)parentResponder();
    tableView->scrollToSubviewOfTypeOfCellAtLocation(otherSubviewType, tableView->selectedColumn(), tableView->selectedRow());
    HistoryViewCell * selectedCell = (HistoryViewCell *)(tableView->selectedCell());
    m_dataSource->setSelectedSubviewType(otherSubviewType, selectedCell);
    app()->setFirstResponder(selectedCell);
    return true;
  }
  return false;
}

}
