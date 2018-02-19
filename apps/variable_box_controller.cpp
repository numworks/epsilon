#include "variable_box_controller.h"
#include "constant.h"
#include <escher/metric.h>
#include <assert.h>

using namespace Poincare;

/* ContentViewController */

VariableBoxController::ContentViewController::ContentViewController(Responder * parentResponder, GlobalContext * context) :
  ViewController(parentResponder),
  m_context(context),
  m_textFieldCaller(nullptr),
  m_firstSelectedRow(0),
  m_previousSelectedRow(0),
  m_currentPage(Page::RootMenu),
  m_selectableTableView(this, this, 0, 1, 0, 0, 0, 0, this, nullptr, false)
{
}

const char * VariableBoxController::ContentViewController::title() {
  return I18n::translate(I18n::Message::Variables);
}

View * VariableBoxController::ContentViewController::view() {
  return &m_selectableTableView;
}

void VariableBoxController::ContentViewController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  m_selectableTableView.scrollToCell(0,0);
  selectCellAtLocation(0, m_firstSelectedRow);
  app()->setFirstResponder(&m_selectableTableView);
}

bool VariableBoxController::ContentViewController::handleEvent(Ion::Events::Event event) {
#if MATRIX_VARIABLES
  if (event == Ion::Events::Back && m_currentPage == Page::RootMenu) {
#else
  if (event == Ion::Events::Back && m_currentPage == Page::Scalar) {
#endif
    m_firstSelectedRow = 0;
    app()->dismissModalViewController();
    return true;
  }
  if (event == Ion::Events::Back || event == Ion::Events::Left) {
    m_firstSelectedRow = m_previousSelectedRow;
#if MATRIX_VARIABLES
    m_selectableTableView.deselectTable();
    m_currentPage = Page::RootMenu;
#endif
    app()->setFirstResponder(this);
    return true;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && m_currentPage == Page::RootMenu)) {
  if (m_currentPage == Page::RootMenu) {
      m_previousSelectedRow = selectedRow();
      m_firstSelectedRow = 0;
      m_selectableTableView.deselectTable();
      m_currentPage = pageAtIndex(m_previousSelectedRow);
      app()->setFirstResponder(this);
      return true;
    }
    m_firstSelectedRow = 0;
    char label[3];
    putLabelAtIndexInBuffer(selectedRow(), label);
    const char * editedText = label;
    m_textFieldCaller->handleEventWithText(editedText);
#if MATRIX_VARIABLES
    m_selectableTableView.deselectTable();
    m_currentPage = Page::RootMenu;
#endif
    app()->dismissModalViewController();
    return true;
  }
  if (event == Ion::Events::Backspace && m_currentPage != Page::RootMenu) {
    if (m_currentPage == Page::Scalar) {
      const Symbol symbol('A'+selectedRow());
      m_context->setExpressionForSymbolName(nullptr, &symbol, *m_context);
    }
    if (m_currentPage == Page::Matrix) {
      const Symbol symbol = Symbol::matrixSymbol('0'+(char)selectedRow());
      m_context->setExpressionForSymbolName(nullptr, &symbol, *m_context);
    }
    m_selectableTableView.reloadData();
    return true;
  }
  return false;
}

int VariableBoxController::ContentViewController::numberOfRows() {
  switch (m_currentPage) {
    case Page::RootMenu:
      return k_numberOfMenuRows;
    case Page::Scalar:
      return GlobalContext::k_maxNumberOfScalarExpressions;
#if LIST_VARIABLES
    case Page::List:
      return GlobalContext::k_maxNumberOfListExpressions;
#endif
    case Page::Matrix:
      return GlobalContext::k_maxNumberOfMatrixExpressions;
    default:
      return 0;
  }
}

HighlightCell * VariableBoxController::ContentViewController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  if (type == 0) {
    assert(index < k_maxNumberOfDisplayedRows);
    return &m_leafCells[index];
  }
  assert(index < k_numberOfMenuRows);
  return &m_nodeCells[index];
}

int VariableBoxController::ContentViewController::reusableCellCount(int type) {
  assert(type < 2);
  if (type == 0) {
    return k_maxNumberOfDisplayedRows;
  }
  return k_numberOfMenuRows;
}

void VariableBoxController::ContentViewController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (m_currentPage == Page::RootMenu) {
    I18n::Message label = nodeLabelAtIndex(index);
    MessageTableCell * myCell = (MessageTableCell *)cell;
    myCell->setMessage(label);
    return;
  }
  VariableBoxLeafCell * myCell = (VariableBoxLeafCell *)cell;
  char label[3];
  putLabelAtIndexInBuffer(index, label);
  myCell->setLabel(label);
  const Expression * evaluation = expressionForIndex(index);
  if (m_currentPage == Page::Scalar) {
    myCell->displayExpression(false);
    char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
    evaluation->writeTextInBuffer(buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits));
    myCell->setSubtitle(buffer);
    return;
  }
  myCell->displayExpression(true);
  if (evaluation) {
    /* TODO: implement list contexts */
    // TODO: handle matrix and scalar!
    const Matrix * matrixEvaluation = static_cast<const Matrix *>(evaluation);
    myCell->setExpression(matrixEvaluation);
    char buffer[2*PrintFloat::bufferSizeForFloatsWithPrecision(2)+1];
    int numberOfChars = PrintFloat::convertFloatToText<float>(matrixEvaluation->numberOfRows(), buffer, PrintFloat::bufferSizeForFloatsWithPrecision(2), 2, PrintFloat::Mode::Decimal);
    buffer[numberOfChars++] = 'x';
    PrintFloat::convertFloatToText<float>(matrixEvaluation->numberOfColumns(), buffer+numberOfChars, PrintFloat::bufferSizeForFloatsWithPrecision(2), 2, PrintFloat::Mode::Decimal);
    myCell->setSubtitle(buffer);
  } else {
    myCell->setExpression(nullptr);
    myCell->setSubtitle(I18n::translate(I18n::Message::Empty));
  }
}

KDCoordinate VariableBoxController::ContentViewController::rowHeight(int index) {
  if (m_currentPage == Page::RootMenu || m_currentPage == Page::Scalar) {
    return Metric::ToolboxRowHeight;
  }
  const Expression * expression = expressionForIndex(index);
  if (expression) {
    ExpressionLayout * layout = expression->createLayout();
    KDCoordinate expressionHeight = layout->size().height();
    delete layout;
    return expressionHeight+k_leafMargin;
  }
  return Metric::ToolboxRowHeight;
}

KDCoordinate VariableBoxController::ContentViewController::cumulatedHeightFromIndex(int j) {
  int result = 0;
  for (int k = 0; k < j; k++) {
    result += rowHeight(k);
  }
  return result;
}

int VariableBoxController::ContentViewController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  while (result < offsetY && j < numberOfRows()) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

int VariableBoxController::ContentViewController::typeAtLocation(int i, int j) {
  if (m_currentPage == Page::RootMenu) {
    return 1;
  }
  return 0;
}

const Expression * VariableBoxController::ContentViewController::expressionForIndex(int index) {
  if (m_currentPage == Page::Scalar) {
    const Symbol symbol = Symbol('A'+index);
    return m_context->expressionForSymbol(&symbol);
  }
  if (m_currentPage == Page::Matrix) {
    const Symbol symbol = Symbol::matrixSymbol('0'+(char)index);
    return m_context->expressionForSymbol(&symbol);
  }
#if LIST_VARIABLES
  if (m_currentPage == Page::List) {
    return nullptr;
  }
#endif
  return nullptr;
}

VariableBoxController::ContentViewController::Page VariableBoxController::ContentViewController::pageAtIndex(int index) {
#if LIST_VARIABLES
  Page pages[3] = {Page::Scalar, Page::List, Page::Matrix};
#else
  Page pages[2] = {Page::Scalar, Page::Matrix};
#endif
  return pages[index];
}

void VariableBoxController::ContentViewController::putLabelAtIndexInBuffer(int index, char * buffer) {
  if (m_currentPage == Page::Scalar) {
    buffer[0] = 'A' + index;
    buffer[1] = 0;
  }
#if LIST_VARIABLES
  if (m_currentPage == Page::List) {
    buffer[0] = 'L';
    buffer[1] = '0' + index;
    buffer[2] = 0;
  }
#endif
  if (m_currentPage == Page::Matrix) {
    buffer[0] = 'M';
    buffer[1] = '0' + index;
    buffer[2] = 0;
  }
}

I18n::Message VariableBoxController::ContentViewController::nodeLabelAtIndex(int index) {
  assert(m_currentPage == Page::RootMenu);
#if LIST_VARIABLES
  I18n::Message labels[3] = {I18n::Message::Number, I18n::Message::List, I18n::Message::Matrix};
#else
  I18n::Message labels[2] = {I18n::Message::Number, I18n::Message::Matrix};
#endif
  return labels[index];
}

void VariableBoxController::ContentViewController::setTextFieldCaller(TextField * textField) {
  m_textFieldCaller = textField;
}

void VariableBoxController::ContentViewController::reloadData() {
  m_selectableTableView.reloadData();
}

void VariableBoxController::ContentViewController::resetPage() {
#if MATRIX_VARIABLES
  m_currentPage = Page::RootMenu;
#else
  m_currentPage = Page::Scalar;
#endif
}

void VariableBoxController::ContentViewController::viewDidDisappear() {
  m_selectableTableView.deselectTable();
  ViewController::viewDidDisappear();
}

VariableBoxController::VariableBoxController(GlobalContext * context) :
  StackViewController(nullptr, &m_contentViewController, KDColorWhite, Palette::PurpleBright, Palette::PurpleDark),
  m_contentViewController(this, context)
{
}

void VariableBoxController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_contentViewController);
}

void VariableBoxController::setTextFieldCaller(TextField * textField) {
  m_contentViewController.setTextFieldCaller(textField);
}

void VariableBoxController::viewWillAppear() {
  StackViewController::viewWillAppear();
  m_contentViewController.resetPage();
  m_contentViewController.reloadData();
}

void VariableBoxController::viewDidDisappear() {
  StackViewController::viewDidDisappear();
}
