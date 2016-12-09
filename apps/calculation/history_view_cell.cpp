#include "history_view_cell.h"
#include "../constant.h"
#include "selectable_table_view.h"
#include <assert.h>
#include <string.h>

namespace Calculation {

HistoryViewCell::HistoryViewCell() :
  Responder(nullptr),
  m_prettyPrint(PrettyPrintView(this)),
  m_result(ExpressionView()),
  m_selectedSubviewType(HistoryViewCell::SubviewType::Result)
{
}

KDColor HistoryViewCell::backgroundColor() const {
  KDColor background = m_even ? EvenOddCell::k_evenLineBackgroundColor : EvenOddCell::k_oddLineBackgroundColor;
  return background;
}


int HistoryViewCell::numberOfSubviews() const {
  return 2;
}

View * HistoryViewCell::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_prettyPrint;
    case 1:
      return &m_result;
    default:
      assert(false);
      return nullptr;
  }
}

void HistoryViewCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDSize prettyPrintSize = m_prettyPrint.minimalSizeForOptimalDisplay();
  if (prettyPrintSize.width() + k_digitHorizontalMargin > width) {
    m_prettyPrint.setFrame(KDRect(k_digitHorizontalMargin, k_digitVerticalMargin, width - k_digitHorizontalMargin, prettyPrintSize.height()));
  } else {
    m_prettyPrint.setFrame(KDRect(k_digitHorizontalMargin, k_digitVerticalMargin, prettyPrintSize.width(), prettyPrintSize.height()));
  }
  KDSize resultSize = m_result.minimalSizeForOptimalDisplay();
  KDRect resultFrame(width - resultSize.width() - k_digitHorizontalMargin, prettyPrintSize.height() + 2*k_digitVerticalMargin, resultSize.width(), height - prettyPrintSize.height() - 2*k_digitVerticalMargin);
  m_result.setFrame(resultFrame);
}

void HistoryViewCell::setCalculation(Calculation * calculation) {
  m_prettyPrint.setExpression(calculation->inputLayout());
  m_result.setExpression(calculation->outputLayout());
}

void HistoryViewCell::reloadCell() {
  m_result.setBackgroundColor(backgroundColor());
  m_prettyPrint.setBackgroundColor(backgroundColor());
  if (isHighlighted()) {
    if (m_selectedSubviewType == HistoryViewCell::SubviewType::Result) {
      m_result.setBackgroundColor(Palette::FocusCellBackgroundColor);
    } else {
      m_prettyPrint.setBackgroundColor(Palette::FocusCellBackgroundColor);
    }
  }
  layoutSubviews();
  EvenOddCell::reloadCell();
  m_prettyPrint.reloadCell();
}

void HistoryViewCell::didBecomeFirstResponder() {
  if (m_selectedSubviewType == HistoryViewCell::SubviewType::PrettyPrint) {
    app()->setFirstResponder(&m_prettyPrint);
  }
}

HistoryViewCell::SubviewType HistoryViewCell::selectedSubviewType() {
  return m_selectedSubviewType;
}

void HistoryViewCell::setSelectedSubviewType(HistoryViewCell::SubviewType subviewType) {
  m_selectedSubviewType = subviewType;
}

bool HistoryViewCell::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Down && m_selectedSubviewType == HistoryViewCell::SubviewType::PrettyPrint) {
    CalculationSelectableTableView * tableView = (CalculationSelectableTableView *)parentResponder();
    tableView->scrollToSubviewOfTypeOfCellAtLocation(HistoryViewCell::SubviewType::Result, tableView->selectedColumn(), tableView->selectedRow());
    HistoryViewCell * selectedCell = (HistoryViewCell *)(tableView->selectedCell());
    selectedCell->setSelectedSubviewType(HistoryViewCell::SubviewType::Result);
    app()->setFirstResponder(selectedCell);
    selectedCell->reloadCell();
    return true;
  }
  if (event == Ion::Events::Up && m_selectedSubviewType == HistoryViewCell::SubviewType::Result) {
    CalculationSelectableTableView * tableView = (CalculationSelectableTableView *)parentResponder();
    tableView->scrollToSubviewOfTypeOfCellAtLocation(HistoryViewCell::SubviewType::PrettyPrint, tableView->selectedColumn(), tableView->selectedRow());
    HistoryViewCell * selectedCell = (HistoryViewCell *)(tableView->selectedCell());
    selectedCell->setSelectedSubviewType(HistoryViewCell::SubviewType::PrettyPrint);
    app()->setFirstResponder(selectedCell);
    selectedCell->reloadCell();
    return true;
  }
  return false;
}

}
