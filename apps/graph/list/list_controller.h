#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include <escher.h>
#include "../function_store.h"
#include "function_cell.h"

class ListController : public ViewController, public TableViewDataSource {
public:
  ListController(Responder * parentResponder, Graph::FunctionStore * functionStore);

  void setActiveCell(int index);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;

  int numberOfCells() override;
  void willDisplayCellForIndex(View * cell, int index) override;
  KDCoordinate cellHeight() override;
  View * reusableCell(int index) override;
  int reusableCellCount() override;
private:
  constexpr static int k_totalNumberOfModels = 20;
  constexpr static int k_maxNumberOfCells = 10;
  // !!! CAUTION: The order here is important
  // The cells should be initialized *before* the tableview!
  FunctionCell m_cells[k_maxNumberOfCells];
  TableView m_tableView;
  int m_activeCell;
  KDCoordinate m_manualScrolling;
  Graph::FunctionStore * m_functionStore;
};

#endif
