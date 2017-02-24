#ifndef SHARED_VALUES_CONTROLLER_H
#define SHARED_VALUES_CONTROLLER_H

#include <escher.h>
#include "function_store.h"
#include "function_title_cell.h"
#include "editable_cell_table_view_controller.h"
#include "interval.h"
#include "values_parameter_controller.h"
#include "values_function_parameter_controller.h"
#include "interval_parameter_controller.h"

namespace Shared {

class ValuesController : public EditableCellTableViewController, public HeaderViewDelegate,  public AlternateEmptyViewDelegate {
public:
  ValuesController(Responder * parentResponder, HeaderViewController * header, char symbol);
  const char * title() const override;
  Interval * interval();
  virtual bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  ViewController * intervalParameterController();
  int numberOfButtons() const override;
  Button * buttonAtIndex(int index) override;
  virtual void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  bool isEmpty() const override;
  Responder * defaultController() override;
  void viewWillAppear() override;
  static constexpr KDCoordinate k_topMargin = 10;
  static constexpr KDCoordinate k_bottomMargin = 5;
  static constexpr KDCoordinate k_leftMargin = 1;
  static constexpr KDCoordinate k_rightMargin = 10;
  static constexpr KDCoordinate k_abscissaCellWidth = 150;
  static constexpr KDCoordinate k_ordinateCellWidth = 100;
protected:
  StackViewController * stackController() const;
private:
  virtual Function * functionAtColumn(int i);
  Responder * tabController() const;
  void configureAbscissa();
  void configureFunction();
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  void setDataAtLocation(float floatBody, int columnIndex, int rowIndex) override;
  float dataAtLocation(int columnIndex, int rowIndex) override;
  int numberOfElements() override;
  int maxNumberOfElements() const override;
  constexpr static int k_maxNumberOfAbscissaCells = 10;
  virtual int maxNumberOfCells() = 0;
  virtual int maxNumberOfFunctions() = 0;
  EvenOddPointerTextCell m_abscissaTitleCell;
  virtual FunctionTitleCell * functionTitleCells(int j) = 0;
  virtual EvenOddBufferTextCell * floatCells(int j) = 0;
  char m_draftTextBuffer[EditableTextCell::k_bufferLength];
  EvenOddEditableTextCell m_abscissaCells[k_maxNumberOfAbscissaCells];
  virtual FunctionStore * functionStore() const = 0;
  virtual ValuesFunctionParameterController * functionParameterController() = 0;
  Interval m_interval;
  IntervalParameterController m_intervalParameterController;
  ValuesParameterController m_abscissaParameterController;
  Button m_setIntervalButton;
};

}

#endif

