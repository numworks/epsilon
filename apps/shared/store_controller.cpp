#include "store_controller.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

StoreController::StoreController(Responder * parentResponder, FloatPairStore * store, ButtonRowController * header) :
  EditableCellTableViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_editableCells{},
  m_store(store),
  m_storeParameterController(this, store)
{
}

const char * StoreController::title() {
  return I18n::translate(I18n::Message::DataTab);
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
      return m_editableCells[index];
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
  willDisplayCellAtLocationWithDisplayMode(cell, i, j, PrintFloat::Mode::Decimal);
}

void StoreController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  EditableCellTableViewController::didBecomeFirstResponder();
}

bool StoreController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    selectableTableView()->deselectTable();
    assert(selectedRow() == -1);
    app()->setFirstResponder(tabController());
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && selectedRow() == 0) {
    m_storeParameterController.selectXColumn(selectedColumn() == 0);
    StackViewController * stack = ((StackViewController *)parentResponder()->parentResponder());
    stack->push(&m_storeParameterController);
    return true;
  }
  if (event == Ion::Events::Backspace) {
    if (selectedRow() == 0 || selectedRow() == numberOfRows()-1) {
      return false;
    }
    m_store->deletePairAtIndex(selectedRow()-1);
    selectableTableView()->reloadData();
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

bool StoreController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  m_store->set(floatBody, columnIndex, rowIndex-1);
  return true;
}

double StoreController::dataAtLocation(int columnIndex, int rowIndex) {
  return m_store->get(columnIndex, rowIndex-1);
}

int StoreController::numberOfElements() {
  return m_store->numberOfPairs();
}

int StoreController::maxNumberOfElements() const {
  return FloatPairStore::k_maxNumberOfPairs;
}

View * StoreController::loadView() {
  SelectableTableView * tableView = (SelectableTableView*)EditableCellTableViewController::loadView();
  for (int i = 0; i < k_maxNumberOfEditableCells; i++) {
    m_editableCells[i] = new EvenOddEditableTextCell(tableView, this, m_draftTextBuffer);
  }
  return tableView;
}

void StoreController::unloadView(View * view) {
  for (int i = 0; i < k_maxNumberOfEditableCells; i++) {
    delete m_editableCells[i];
    m_editableCells[i] = nullptr;
  }
  EditableCellTableViewController::unloadView(view);
}

}
