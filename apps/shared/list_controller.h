#ifndef SHARED_LIST_CONTROLLER_H
#define SHARED_LIST_CONTROLLER_H

#include <escher.h>
#include "function_store.h"
#include "list_parameter_controller.h"
#include "new_function_cell.h"

namespace Shared {

class ListController : public ViewController, public HeaderViewDelegate, public TableViewDataSource {
public:
  ListController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header);
  View * view() override;
  int numberOfRows() override;
  int numberOfColumns() override;
  virtual KDCoordinate rowHeight(int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  int typeAtLocation(int i, int j) override;
  TableViewCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
protected:
  static constexpr KDCoordinate k_emptyRowHeight = 50;
  bool addFunction();
  void configureFunction(Function * function);
  StackViewController * stackController() const;
  SelectableTableView m_selectableTableView;
  FunctionStore * m_functionStore;
private:
  static constexpr KDCoordinate k_functionNameWidth = 65;
  Responder * tabController() const;
  virtual ListParameterController * parameterController() = 0;
  virtual int maxNumberOfRows() = 0;
  virtual TableViewCell * titleCells(int index) = 0;
  virtual TableViewCell * expressionCells(int index) = 0;
  virtual void willDisplayTitleCellAtIndex(TableViewCell * cell, int j) = 0;
  virtual void willDisplayExpressionCellAtIndex(TableViewCell * cell, int j) = 0;
  EvenOddCell m_emptyCell;
  NewFunctionCell m_addNewFunction;
};

}

#endif
