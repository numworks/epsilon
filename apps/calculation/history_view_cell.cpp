#include "history_view_cell.h"
#include "app.h"
#include "../constant.h"
#include "selectable_table_view.h"
#include <assert.h>
#include <string.h>

namespace Calculation {

static inline KDCoordinate minCoordinate(KDCoordinate x, KDCoordinate y) { return x < y ? x : y; }
static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

/* HistoryViewCellDataSource */

HistoryViewCellDataSource::HistoryViewCellDataSource() :
  m_selectedSubviewType(SubviewType::Output) {}

void HistoryViewCellDataSource::setSelectedSubviewType(SubviewType subviewType) {
  m_selectedSubviewType = subviewType;
  HistoryViewCell * cell = historyViewCellDidChangeSelection();
  if (cell) {
    cell->setHighlighted(cell->isHighlighted());
    cell->cellDidSelectSubview(subviewType);
  }
}

/* HistoryViewCell */

HistoryViewCell::HistoryViewCell(Responder * parentResponder) :
  Responder(parentResponder),
  m_calculationDisplayOutput(Calculation::DisplayOutput::Unknown),
  m_calculationExpanded(false),
  m_inputView(this),
  m_scrollableOutputView(this)
{
  m_calculationCRC32 = 0;
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
}

Poincare::Layout HistoryViewCell::layout() const {
  assert(m_dataSource);
  if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
    return m_inputView.layout();
  } else {
    return m_scrollableOutputView.layout();
  }
}

void HistoryViewCell::reloadScroll() {
  m_inputView.reloadScroll();
  m_scrollableOutputView.reloadScroll();
}

void HistoryViewCell::reloadOutputSelection() {
  /* Select the right output according to the calculation display output. This
   * will reload the scroll to display the selected output. */
  if (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximate) {
    m_scrollableOutputView.setSelectedSubviewPosition(Shared::ScrollableExactApproximateExpressionsView::SubviewPosition::Left);
  } else {
    assert((m_calculationDisplayOutput == Calculation::DisplayOutput::ApproximateOnly)
        || (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximateToggle)
        || (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactOnly));
    m_scrollableOutputView.setSelectedSubviewPosition(Shared::ScrollableExactApproximateExpressionsView::SubviewPosition::Right);
  }
}

void HistoryViewCell::cellDidSelectSubview(HistoryViewCellDataSource::SubviewType type) {
  // Init output selection
  if (type == HistoryViewCellDataSource::SubviewType::Output) {
    reloadOutputSelection();
  }

  /* The selected subview has changed. The displayed outputs might have changed.
   * For example, for the calculation 1.2+2 --> 3.2, selecting the output would
   * display 1.2+2 --> 16/5 = 3.2. */
  m_calculationExpanded = (type == HistoryViewCellDataSource::SubviewType::Output);
  m_scrollableOutputView.setDisplayLeftLayout(displayLeftLayout());

  /* The displayed outputs have changed. We need to re-layout the cell
   * and re-initialize the scroll. */
  layoutSubviews();
  reloadScroll();
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
    minCoordinate(maxFrameWidth, inputSize.width()),
    inputSize.height()
  ));
  KDSize outputSize = m_scrollableOutputView.minimalSizeForOptimalDisplay();
  m_scrollableOutputView.setFrame(KDRect(
    maxCoordinate(0, maxFrameWidth - outputSize.width()),
    inputSize.height(),
    minCoordinate(maxFrameWidth, outputSize.width()),
    outputSize.height()
  ));
}

void HistoryViewCell::setCalculation(Calculation * calculation, bool expanded) {
  uint32_t newCalculationCRC = Ion::crc32Byte((const uint8_t *)calculation, ((char *)calculation->next()) - ((char *) calculation));
  if (m_calculationExpanded == expanded && newCalculationCRC == m_calculationCRC32) {
    return;
  }
  Poincare::Context * context = App::app()->localContext();

  // Clean the layouts to make room in the pool
  m_inputView.setLayout(Poincare::Layout());
  m_scrollableOutputView.setLayouts(Poincare::Layout(), Poincare::Layout());

  // Memoization
  m_calculationCRC32 = newCalculationCRC;
  m_calculationExpanded = expanded;
  m_calculationDisplayOutput = calculation->displayOutput(context);
  m_inputView.setLayout(calculation->createInputLayout());
  /* Both output expressions have to be updated at the same time. Otherwise,
   * when updating one layout, if the second one still points to a deleted
   * layout, calling to layoutSubviews() would fail. */
  Poincare::Layout leftOutputLayout = calculation->createExactOutputLayout();
  Poincare::Layout rightOutputLayout = (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactOnly) ? leftOutputLayout :
    calculation->createApproximateOutputLayout(context);
  m_scrollableOutputView.setDisplayLeftLayout(displayLeftLayout()); // Must be before the setLayouts fo the reload
  m_scrollableOutputView.setLayouts(rightOutputLayout, leftOutputLayout);
  I18n::Message equalMessage = calculation->exactAndApproximateDisplayedOutputsAreEqual(context) == Calculation::EqualSign::Equal ? I18n::Message::Equal : I18n::Message::AlmostEqual;
  m_scrollableOutputView.setEqualMessage(equalMessage);

  /* The displayed input and outputs have changed. We need to re-layout the cell
   * and re-initialize the scroll. */
  layoutSubviews();
  reloadScroll();
}

void HistoryViewCell::didBecomeFirstResponder() {
  assert(m_dataSource);
  if (m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) {
    Container::activeApp()->setFirstResponder(&m_inputView);
  } else {
    Container::activeApp()->setFirstResponder(&m_scrollableOutputView);
  }
}

bool HistoryViewCell::handleEvent(Ion::Events::Event event) {
  assert(m_dataSource);
  if ((event == Ion::Events::Down && m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input) ||
    (event == Ion::Events::Up && m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Output)) {
    HistoryViewCellDataSource::SubviewType otherSubviewType = m_dataSource->selectedSubviewType() == HistoryViewCellDataSource::SubviewType::Input ? HistoryViewCellDataSource::SubviewType::Output : HistoryViewCellDataSource::SubviewType::Input;
    m_dataSource->setSelectedSubviewType(otherSubviewType);
    CalculationSelectableTableView * tableView = (CalculationSelectableTableView *)parentResponder();
    tableView->scrollToSubviewOfTypeOfCellAtLocation(otherSubviewType, tableView->selectedColumn(), tableView->selectedRow());
    Container::activeApp()->setFirstResponder(this);
    return true;
  }
  return false;
}

bool HistoryViewCell::displayLeftLayout() const {
  return (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximate)
    || (m_calculationDisplayOutput == Calculation::DisplayOutput::ExactAndApproximateToggle && m_calculationExpanded);
}

}
