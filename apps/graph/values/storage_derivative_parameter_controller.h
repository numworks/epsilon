#ifndef GRAPH_STORAGE_DERIVATIVE_PARAM_CONTROLLER_H
#define GRAPH_STORAGE_DERIVATIVE_PARAM_CONTROLLER_H

#include <escher.h>
#include "../../shared/storage_cartesian_function.h"

namespace Graph {

class StorageValuesController;

class StorageDerivativeParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  StorageDerivativeParameterController(StorageValuesController * valuesController);

  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void setFunction(Shared::StorageCartesianFunction * function) {
    m_function = function;
  }
private:
#if COPY_COLUMN
  constexpr static int k_totalNumberOfCell = 2;
#else
  constexpr static int k_totalNumberOfCell = 1;
#endif
  constexpr static int k_maxNumberOfCharsInTitle = Shared::StorageFunction::k_maxNameWithArgumentSize + 1; // +1 for the ' of the derivative
  char m_pageTitle[k_maxNumberOfCharsInTitle];
  MessageTableCell m_hideColumn;
#if COPY_COLUMN
  MessageTableCellWithChevron m_copyColumn;
#endif
  SelectableTableView m_selectableTableView;
  Shared::StorageCartesianFunction * m_function;
  StorageValuesController * m_valuesController;
};

}

#endif
