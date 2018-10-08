#include "variable_box_controller.h"
#include "shared/poincare_helpers.h"
#include "constant.h"
#include <escher/metric.h>
#include <assert.h>
#include <poincare/matrix_layout.h>
#include <poincare/layout_helper.h>

using namespace Poincare;
using namespace Shared;
using namespace Ion;

VariableBoxController::VariableBoxController() :
  NestedMenuController(nullptr, I18n::Message::Variables),
  m_currentPage(Page::RootMenu)
{
}

void VariableBoxController::viewWillAppear() {
  StackViewController::viewWillAppear();
  m_currentPage = Page::RootMenu;
  m_selectableTableView.reloadData();
}

bool VariableBoxController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Backspace && m_currentPage != Page::RootMenu) {
    Storage::Record record = recordAtIndex(selectedRow());
    record.destroy();
    m_selectableTableView.reloadData();
    return true;
  }
  return NestedMenuController::handleEvent(event);
}

int VariableBoxController::numberOfRows() {
  switch (m_currentPage) {
    case Page::RootMenu:
      return k_numberOfMenuRows;
    case Page::Expression:
      return Storage::sharedStorage()->numberOfRecordsWithExtension(GlobalContext::expExtension);
    case Page::Function:
      return Storage::sharedStorage()->numberOfRecordsWithExtension(GlobalContext::funcExtension);
    default:
      return 0;
  }
}

int VariableBoxController::reusableCellCount(int type) {
  assert(type < 2);
  if (type == 0) {
    return k_maxNumberOfDisplayedRows;
  }
  return k_numberOfMenuRows;
}

void VariableBoxController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (m_currentPage == Page::RootMenu) {
    I18n::Message label = nodeLabelAtIndex(index);
    MessageTableCell * myCell = (MessageTableCell *)cell;
    myCell->setMessage(label);
    return;
  }
  ExpressionTableCellWithExpression * myCell = (ExpressionTableCellWithExpression *)cell;
  Storage::Record record = recordAtIndex(index);
  char truncatedName[SymbolAbstract::k_maxNameSize];
  size_t nameLength = SymbolAbstract::TruncateExtension(truncatedName, record.fullName(), SymbolAbstract::k_maxNameSize);
  Layout symbolLayout = LayoutHelper::String(truncatedName, nameLength);
  myCell->setLayout(symbolLayout);
  myCell->setAccessoryLayout(expressionLayoutForIndex(selectedRow()));
}

KDCoordinate VariableBoxController::rowHeight(int index) {
  if (m_currentPage != Page::RootMenu) {
    Layout layoutR = expressionLayoutForIndex(index);
    if (!layoutR.isUninitialized()) {
      return max(layoutR.layoutSize().height()+k_leafMargin, Metric::ToolboxRowHeight);
    }
  }
  return NestedMenuController::rowHeight(index);
}

int VariableBoxController::typeAtLocation(int i, int j) {
  if (m_currentPage == Page::RootMenu) {
    return 1;
  }
  return 0;
}

ExpressionTableCellWithExpression * VariableBoxController::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

MessageTableCellWithChevron * VariableBoxController::nodeCellAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfMenuRows);
  return &m_nodeCells[index];
}

VariableBoxController::Page VariableBoxController::pageAtIndex(int index) {
  Page pages[2] = {Page::Expression, Page::Function};
  return pages[index];
}

bool VariableBoxController::selectSubMenu(int selectedRow) {
  m_selectableTableView.deselectTable();
  m_currentPage = pageAtIndex(selectedRow);
  return NestedMenuController::selectSubMenu(selectedRow);
}

bool VariableBoxController::returnToPreviousMenu() {
  m_selectableTableView.deselectTable();
  m_currentPage = Page::RootMenu;
  return NestedMenuController::returnToPreviousMenu();
}

bool VariableBoxController::selectLeaf(int selectedRow) {
  m_selectableTableView.deselectTable();
  Storage::Record record = recordAtIndex(selectedRow);
  char truncatedName[SymbolAbstract::k_maxNameSize];
  SymbolAbstract::TruncateExtension(truncatedName, record.fullName(), SymbolAbstract::k_maxNameSize);
  sender()->handleEventWithText(truncatedName);
  app()->dismissModalViewController();
  return true;
}

I18n::Message VariableBoxController::nodeLabelAtIndex(int index) {
  assert(m_currentPage == Page::RootMenu);
  I18n::Message labels[2] = {I18n::Message::Expressions, I18n::Message::Functions};
  return labels[index];
}

Layout VariableBoxController::expressionLayoutForIndex(int index) {
  assert(m_currentPage != Page::RootMenu);
  Storage::Record record = recordAtIndex(index);
  return Expression::ExpressionFromRecord(record).createLayout(Poincare::Preferences::sharedPreferences()->displayMode(), Constant::ShortNumberOfSignificantDigits);
}

const char * VariableBoxController::extension() const {
  return m_currentPage == Page::Function ? GlobalContext::funcExtension : GlobalContext::expExtension;
}

Storage::Record VariableBoxController::recordAtIndex(int rowIndex) {
  return Storage::sharedStorage()->recordWithExtensionAtIndex(extension(), rowIndex);
}

/* void VariableBoxController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
  // Tidy the memoized layouts to clean TreePool
  for (int i = 0; i < 10; i++) { // FIXME
    m_matrixLayouts[i] = Layout();
  }
  // Tidy the layouts used to display the VariableBoxController to clean TreePool
  for (int i = 0; i < k_maxNumberOfDisplayedRows; i++) {
    m_leafCells[i].setLayout(Layout());
  }
  ViewController::viewDidDisappear();
  }


Layout VariableBoxController::matrixLayoutAtIndex(int index) {
  assert(m_currentPage == Page::Matrix);
  if (m_matrixLayouts[index].isUninitialized()) {
    const Expression evaluation = expressionForIndex(index);
    if (!evaluation.isUninitialized()) {
      m_matrixLayouts[index] = evaluation.createLayout(Poincare::Preferences::sharedPreferences()->displayMode(), Constant::ShortNumberOfSignificantDigits);
    }
  }
  return m_matrixLayouts[index];
}

}*/

