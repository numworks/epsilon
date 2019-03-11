#ifndef GRAPH_STORAGE_DERIVATIVE_PARAM_CONTROLLER_H
#define GRAPH_STORAGE_DERIVATIVE_PARAM_CONTROLLER_H

#include <escher.h>
#include "../storage_cartesian_function_store.h"

namespace Graph {

class StorageValuesController;

class StorageDerivativeParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  StorageDerivativeParameterController(StorageValuesController * valuesController);

  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void setRecord(Ion::Storage::Record record) {
    m_record = record;
  }
private:
  StorageCartesianFunctionStore * functionStore();
#if COPY_COLUMN
  constexpr static int k_totalNumberOfCell = 2;
#else
  constexpr static int k_totalNumberOfCell = 1;
#endif
  constexpr static int k_maxNumberOfCharsInTitle = Shared::Function::k_maxNameWithArgumentSize + 1; // +1 for the ' of the derivative
  char m_pageTitle[k_maxNumberOfCharsInTitle];
  MessageTableCell m_hideColumn;
#if COPY_COLUMN
  MessageTableCellWithChevron m_copyColumn;
#endif
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
  StorageValuesController * m_valuesController;
};

}

#endif
