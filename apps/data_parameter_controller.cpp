#include "data_parameter_controller.h"
#include <assert.h>

DataParameterController::DataParameterController(Responder * parentResponder, Data * data) :
  ViewController(parentResponder),
  m_deleteColumn(MenuListCell((char*)"Effacer la colonne")),
  m_copyColumn(ChevronMenuListCell((char*)"Copier la colonne dans une liste")),
  m_importList(ChevronMenuListCell((char*)"Importer une liste")),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_data(data),
  m_xColumnSelected(true)
{
}

void DataParameterController::selectXColumn(bool xColumnSelected) {
  m_xColumnSelected = xColumnSelected;
}


const char * DataParameterController::title() const {
  return "Option de la colonne";
}

View * DataParameterController::view() {
  return &m_selectableTableView;
}

void DataParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool DataParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    switch (m_selectableTableView.selectedRow()) {
      case 0:
      {
        if (m_xColumnSelected) {
          m_data->deleteAllXValues();
        } else {
          m_data->deleteAllYValues();
        }
        StackViewController * stack = ((StackViewController *)parentResponder());
        stack->pop();
        return true;
      }
      case 1:
        return false;
      case 2:
        return false;
      default:
        assert(false);
        return false;
    }
  }
  return false;
}

int DataParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * DataParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  TableViewCell * cells[] = {&m_deleteColumn, &m_copyColumn, &m_importList};
  return cells[index];
}

int DataParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate DataParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}
