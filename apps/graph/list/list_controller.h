#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include <escher.h>
#include "../function_store.h"
#include "../function_title_cell.h"
#include "../../shared/function_expression_cell.h"
#include "../../shared/new_function_cell.h"
#include "../../shared/list_parameter_controller.h"

namespace Graph {

class ListController : public ViewController, public HeaderViewDelegate, public TableViewDataSource {
public:
  ListController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header);
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  bool handleEnter();
  void didBecomeFirstResponder() override;
  virtual View * view() override;

  int numberOfRows() override;
  int numberOfColumns() override;
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  TableViewCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void configureFunction(Function * function);
  void editExpression(Shared::FunctionExpressionCell * functionCell, Ion::Events::Event event);

private:
  static constexpr KDCoordinate k_functionNameWidth = 65;
  static constexpr KDCoordinate k_emptyRowHeight = 50;
  Responder * tabController() const;
  StackViewController * stackController() const;
  constexpr static int k_maxNumberOfRows = 5;
  // !!! CAUTION: The order here is important
  // The cells should be initialized *before* the TableView!
  FunctionTitleCell m_functionTitleCells[k_maxNumberOfRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfRows];
  EvenOddCell m_emptyCell;
  Shared::NewFunctionCell m_addNewFunction;
  SelectableTableView m_selectableTableView;
  FunctionStore * m_functionStore;
  Shared::ListParameterController m_parameterController;
};

}

#endif
