#ifndef GRAPH_VALUES_CONTROLLER_H
#define GRAPH_VALUES_CONTROLLER_H

#include <escher.h>
#include "../function_store.h"
#include "../function_title_cell.h"
#include "../../editable_cell_table_view_controller.h"
#include "interval.h"
#include "abscissa_parameter_controller.h"
#include "derivative_parameter_controller.h"
#include "function_parameter_controller.h"
#include "interval_parameter_controller.h"

namespace Graph {

class ValuesController : public EditableCellTableViewController, public HeaderViewDelegate,  public AlternateEmptyViewDelegate {
public:
  ValuesController(Responder * parentResponder, FunctionStore * functionStore, HeaderViewController * header);
  const char * title() const override;
  View * view() override;
  Interval * interval();
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  ViewController * intervalParameterController();
  int numberOfButtons() const override;
  Button * buttonAtIndex(int index) override;
  int numberOfColumns() override;
  void willDisplayCellAtLocation(TableViewCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  TableViewCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  bool isEmpty() const override;
  const char * emptyMessage() override;
  Responder * defaultController() override;
  void hasChangedTableData();
  void selectCellAtLocation(int i, int j);
  int activeRow();
  int activeColumn();
  static constexpr KDCoordinate k_topMargin = 10;
  static constexpr KDCoordinate k_bottomMargin = 5;
  static constexpr KDCoordinate k_leftMargin = 1;
  static constexpr KDCoordinate k_rightMargin = 10;
  static constexpr KDCoordinate k_abscissaCellWidth = 150;
  static constexpr KDCoordinate k_ordinateCellWidth = 100;
private:
  Function * functionAtColumn(int i);
  bool isDerivativeColumn(int i);
  Responder * tabController() const;
  StackViewController * stackController() const;
  void configureAbscissa();
  void configureFunction();
  void configureDerivativeFunction();
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  void setDataAtLocation(float floatBody, int columnIndex, int rowIndex) override;
  float dataAtLocation(int columnIndex, int rowIndex) override;
  int numberOfElements() override;
  int maxNumberOfElements() const override;
  constexpr static int k_maxNumberOfAbscissaCells = 10;
  constexpr static int k_maxNumberOfCells = 50;
  constexpr static int k_maxNumberOfFunctions = 5;
  EvenOddPointerTextCell m_abscissaTitleCell;
  FunctionTitleCell m_functionTitleCells[k_maxNumberOfFunctions];
  EvenOddBufferTextCell m_floatCells[k_maxNumberOfCells];
  char m_draftTextBuffer[EditableTextCell::k_bufferLength];
  EvenOddEditableTextCell m_abscissaCells[k_maxNumberOfAbscissaCells];
  FunctionStore * m_functionStore;
  Interval m_interval;
  IntervalParameterController m_intervalParameterController;
  AbscissaParameterController m_abscissaParameterController;
  FunctionParameterController m_functionParameterController;
  DerivativeParameterController m_derivativeParameterController;
  Button m_setIntervalButton;
  Expression::DisplayMode m_displayModeVersion;
};

}

#endif
