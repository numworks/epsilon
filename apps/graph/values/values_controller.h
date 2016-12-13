#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include <escher.h>
#include "../function_store.h"
#include "../function_title_cell.h"
#include "value_cell.h"
#include "editable_value_cell.h"
#include "title_cell.h"
#include "interval.h"
#include "abscissa_parameter_controller.h"
#include "derivative_parameter_controller.h"
#include "function_parameter_controller.h"
#include "interval_parameter_controller.h"

namespace Graph {

class ValuesController : public ViewController, public HeaderViewDelegate, public TableViewDataSource, public AlternateEmptyViewDelegate, public SelectableTableViewDelegate, public TextFieldDelegate {
public:
  ValuesController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header);
  Interval * interval();
  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text) override;

  ViewController * intervalParameterController();
  int numberOfButtons() const override;
  Button * buttonAtIndex(int index) override;

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

  bool isEmpty() override;
  const char * emptyMessage() override;
  Responder * defaultController() override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;


  static constexpr KDCoordinate k_topMargin = 10;
  static constexpr KDCoordinate k_bottomMargin = 5;
  static constexpr KDCoordinate k_leftMargin = 1;
  static constexpr KDCoordinate k_rightMargin = 10;
  static constexpr KDCoordinate k_cellHeight = 30;
  static constexpr KDCoordinate k_abscissaCellWidth = 150;
  static constexpr KDCoordinate k_ordinateCellWidth = 100;

private:
  int activeRow();
  int activeColumn();
  Function * functionAtColumn(int i);
  bool isDerivativeColumn(int i);
  Responder * tabController() const;
  StackViewController * stackController() const;
  void configureAbscissa();
  void configureFunction();
  void configureDerivativeFunction();
  constexpr static int k_maxNumberOfAbscissaCells = 8;
  constexpr static int k_maxNumberOfCells = 40;
  constexpr static int k_maxNumberOfFunctions = 5;
  // !!! CAUTION: The order here is important
  // The cells should be initialized *before* the TableView!
  SelectableTableView m_selectableTableView;
  TitleCell m_abscissaTitleCell;
  FunctionTitleCell m_functionTitleCells[k_maxNumberOfFunctions];
  ValueCell m_floatCells[k_maxNumberOfCells];
  EditableValueCell m_abscissaCells[k_maxNumberOfAbscissaCells];
  FunctionStore * m_functionStore;
  Interval m_interval;
  IntervalParameterController m_intervalParameterController;
  AbscissaParameterController m_abscissaParameterController;
  FunctionParameterController m_functionParameterController;
  DerivativeParameterController m_derivativeParameterController;
  Button m_setIntervalButton;
};

}

#endif
