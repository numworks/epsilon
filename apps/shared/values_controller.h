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
#include <apps/i18n.h>

namespace Shared {

class ValuesController : public EditableCellTableViewController, public ButtonRowDelegate,  public AlternateEmptyViewDefaultDelegate {
public:
  ValuesController(Responder * parentResponder, ButtonRowController * header);
  const char * title() override;
  int numberOfColumns() const override;
  virtual bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  virtual IntervalParameterController * intervalParameterController() = 0;
  int numberOfButtons(ButtonRowController::Position) const override;
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
  static constexpr KDCoordinate k_abscissaCellWidth = 108;
  static constexpr KDCoordinate k_ordinateCellWidth = 108;

protected:
  static constexpr int k_abscissaTitleCellType = 0;
  static constexpr int k_functionTitleCellType = 1;
  static constexpr int k_editableValueCellType = 2;
  static constexpr int k_notEditableValueCellType = 3;
  constexpr static int k_maxNumberOfRows = 10;

  static constexpr const KDFont * k_font = KDFont::SmallFont;
  void setupAbscissaCellsAndTitleCells(InputEventHandlerDelegate * inputEventHandlerDelegate);
  StackViewController * stackController() const;
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  virtual void updateNumberOfColumns() const;
  virtual FunctionStore * functionStore() const;
  virtual Ion::Storage::Record recordAtColumn(int i);
  int numberOfElementsInColumn(int columnIndex) const override;
  SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
  mutable int m_numberOfColumns;
  mutable bool m_numberOfColumnsNeedUpdate;
  SelectableTableView m_selectableTableView;
private:
  virtual void setStartEndMessages(Shared::IntervalParameterController * controller, int column) = 0;
  Responder * tabController() const override;
  bool cellAtLocationIsEditable(int columnIndex, int rowIndex) override;
  double dataAtLocation(int columnIndex, int rowIndex) override;
  virtual void printEvaluationOfAbscissaAtColumn(double abscissa, int columnIndex, char * buffer, const int bufferSize) = 0;
  virtual Interval * intervalAtColumn(int columnIndex) = 0;
  virtual I18n::Message valuesParameterMessageAtColumn(int columnIndex) const = 0;
  int maxNumberOfElements() const override {
    return Interval::k_maxNumberOfElements;
  };
  virtual int maxNumberOfCells() = 0;
  virtual int maxNumberOfFunctions() = 0;
  virtual FunctionTitleCell * functionTitleCells(int j) = 0;
  virtual EvenOddBufferTextCell * floatCells(int j) = 0;
  virtual int abscissaCellsCount() const = 0;
  virtual EvenOddEditableTextCell * abscissaCells(int j) = 0;
  virtual int abscissaTitleCellsCount() const = 0;
  virtual EvenOddMessageTextCell * abscissaTitleCells(int j) = 0;
  virtual ViewController * functionParameterController() = 0;
  ValuesParameterController m_abscissaParameterController;
};

}

#endif
