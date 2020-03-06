#ifndef LIST_COLOR_PARAMETER_CONTROLLER_H
#define LIST_COLOR_PARAMETER_CONTROLLER_H

#include <escher.h>
#include <poincare/layout.h>
#include "function_store.h"


namespace Shared {

class ListController;

class ColorParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  ColorParameterController(Responder * parentResponder);
  const char * title() override;
  View * view() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void setRecord(Ion::Storage::Record record) { m_record = record; }
private:
  FunctionStore * functionStore();
  ExpiringPointer<Function> function();
  constexpr static int k_totalNumberOfCells = 8;
  MessageTableCell m_cells[k_totalNumberOfCells]; 
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
  ListController * m_listController;
};

}

#endif
