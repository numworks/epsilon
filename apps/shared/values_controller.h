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
#include "../i18n.h"

namespace Shared {

class ValuesController : public EditableCellTableViewController, public ButtonRowDelegate,  public AlternateEmptyViewDelegate {
public:
  ValuesController(Responder * parentResponder, ButtonRowController * header, I18n::Message parameterTitle, IntervalParameterController * intervalParameterController, Interval * interval);
  const char * title() override;
  Interval * interval();
  int numberOfColumns() override;
  virtual bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  virtual IntervalParameterController * intervalParameterController() = 0;
  int numberOfButtons(ButtonRowController::Position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;
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
  void viewDidDisappear() override;
  static constexpr KDCoordinate k_topMargin = 10;
  static constexpr KDCoordinate k_bottomMargin = 15;
  static constexpr KDCoordinate k_leftMargin = 1;
  static constexpr KDCoordinate k_rightMargin = 15;
  static constexpr KDCoordinate k_abscissaCellWidth = 100;
  static constexpr KDCoordinate k_ordinateCellWidth = 100;
protected:
  StackViewController * stackController() const;
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  View * loadView() override;
  void unloadView(View * view) override;
  virtual void updateNumberOfColumns();
  Interval * m_interval;
  int m_numberOfColumns;
  bool m_numberOfColumnsNeedUpdate;
private:
  virtual Function * functionAtColumn(int i);
  Responder * tabController() const override;
  void configureAbscissa();
  void configureFunction();
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  double dataAtLocation(int columnIndex, int rowIndex) override;
  int numberOfElements() override;
  int maxNumberOfElements() const override;
  virtual double evaluationOfAbscissaAtColumn(double abscissa, int columnIndex);
  constexpr static int k_maxNumberOfAbscissaCells = 10;
  virtual int maxNumberOfCells() = 0;
  virtual int maxNumberOfFunctions() = 0;
  EvenOddMessageTextCell * m_abscissaTitleCell;
  virtual FunctionTitleCell * functionTitleCells(int j) = 0;
  virtual EvenOddBufferTextCell * floatCells(int j) = 0;
  char m_draftTextBuffer[TextField::maxBufferSize()];
  EvenOddEditableTextCell * m_abscissaCells[k_maxNumberOfAbscissaCells];
  virtual FunctionStore * functionStore() const = 0;
  virtual ValuesFunctionParameterController * functionParameterController() = 0;
  ValuesParameterController m_abscissaParameterController;
  Button m_setIntervalButton;
};

}

#endif

