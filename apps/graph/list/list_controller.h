#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include <escher.h>
#include "../function_store.h"
#include "function_name_view.h"
#include "function_expression_view.h"
#include "parameter_controller.h"

class ListController : public ViewController, public TableViewDataSource {
public:
  ListController(Responder * parentResponder, Graph::FunctionStore * functionStore);

  void setActiveCell(int i, int j);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  bool handleEnter();
  void didBecomeFirstResponder() override;

  int numberOfRows() override;
  int numberOfColumns() override;
  void willDisplayCellAtLocation(View * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  View * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void configureFunction(Graph::Function * function);
  int typeAtLocation(int i, int j) override;
private:
  static constexpr KDCoordinate k_minimalfunctionHeight = 40;
  static constexpr KDCoordinate k_functionNameWidth = 40;
  Responder * tabController() const;
  constexpr static int k_maxNumberOfRows = 6;
  // !!! CAUTION: The order here is important
  // The cells should be initialized *before* the TableView!
  FunctionNameView m_nameCells[k_maxNumberOfRows];
  FunctionExpressionView m_expressionCells[k_maxNumberOfRows];
  TableView m_tableView;
  int m_activeCellx;
  int m_activeCelly;
  KDCoordinate m_manualScrolling;
  Graph::FunctionStore * m_functionStore;
  ParameterController m_parameterController;
};

#endif
