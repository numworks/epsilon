#include "store_controller.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

StoreController::StoreController(Responder * parentResponder, FloatPairStore * store, HeaderViewController * header) :
  EditableCellTableViewController(parentResponder, Metric::CommonTopMargin, Metric::CommonRightMargin, Metric::CommonBottomMargin, Metric::CommonLeftMargin),
  HeaderViewDelegate(header),
  m_editableCells{EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer),
    EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer), EvenOddEditableTextCell(&m_selectableTableView, this, m_draftTextBuffer)},
  m_store(store),
  m_storeParameterController(this, store)
{
}

const char * StoreController::title() const {
  return "Donnees";
}

int StoreController::numberOfColumns() {
  return 2;
};

KDCoordinate StoreController::columnWidth(int i) {
  return k_cellWidth;
}

KDCoordinate StoreController::cumulatedWidthFromIndex(int i) {
  return i*k_cellWidth;
}

int StoreController::indexFromCumulatedWidth(KDCoordinate offsetX) {
  return (offsetX-1) / k_cellWidth;
}

HighlightCell * StoreController::reusableCell(int index, int type) {
  assert(index >= 0);
  switch (type) {
    case 0:
      assert(index < k_numberOfTitleCells);
      return titleCells(index);
    case 1:
      assert(index < k_maxNumberOfEditableCells);
      return &m_editableCells[index];
    default:
      assert(false);
      return nullptr;
  }
}

int StoreController::reusableCellCount(int type) {
  if (type == 0) {
    return k_numberOfTitleCells;
  }
  return k_maxNumberOfEditableCells;
}

int StoreController::typeAtLocation(int i, int j) {
  return j!=0;
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, Expression::FloatDisplayMode::Decimal);
}

void StoreController::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() < 0) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  }
  EditableCellTableViewController::didBecomeFirstResponder();
}

bool StoreController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_selectableTableView.deselectTable();
    assert(m_selectableTableView.selectedRow() == -1);
    app()->setFirstResponder(tabController());
    return true;
  }
  if (event == Ion::Events::OK && m_selectableTableView.selectedRow() == 0) {
    m_storeParameterController.selectXColumn(m_selectableTableView.selectedColumn() == 0);
    StackViewController * stack = ((StackViewController *)parentResponder()->parentResponder());
    stack->push(&m_storeParameterController);
    return true;
  }
  if (event == Ion::Events::Backspace) {
    m_store->deletePairAtIndex(m_selectableTableView.selectedRow()-1);
    m_selectableTableView.reloadData();
    return true;
  }
  return false;
}

Responder * StoreController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

bool StoreController::cellAtLocationIsEditable(int columnIndex, int rowIndex) {
  if (rowIndex > 0) {
    return true;
  }
  return false;
}

void StoreController::setDataAtLocation(float floatBody, int columnIndex, int rowIndex) {
  m_store->set(floatBody, columnIndex, rowIndex-1);
}

float StoreController::dataAtLocation(int columnIndex, int rowIndex) {
  return m_store->get(columnIndex, rowIndex-1);
}

int StoreController::numberOfElements() {
  return m_store->numberOfPairs();
}

int StoreController::maxNumberOfElements() const {
  return FloatPairStore::k_maxNumberOfPairs;
}

}
