#ifndef SHARED_LIST_CONTROLLER_H
#define SHARED_LIST_CONTROLLER_H

#include <escher.h>
#include "function_store.h"
#include "list_parameter_controller.h"
#include "new_function_cell.h"
#include "../i18n.h"

namespace Shared {

class ListController : public DynamicViewController, public ButtonRowDelegate, public TableViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate {
public:
  ListController(Responder * parentResponder, FunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer, I18n::Message text);
  int numberOfColumns() override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  int typeAtLocation(int i, int j) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  int numberOfButtons(ButtonRowController::Position position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
protected:
  StackViewController * stackController() const;
  void configureFunction(Function * function);
  virtual void reinitExpression(Function * function);
  SelectableTableView * selectableTableView();
  View * loadView() override;
  void unloadView(View * view) override;
  FunctionStore * m_functionStore;
private:
  static constexpr KDCoordinate k_functionNameWidth = 65;
  TabViewController * tabController() const;
  virtual int functionIndexForRow(int j);
  virtual const char * textForRow(int j);
  virtual void addEmptyFunction();
  virtual bool removeFunctionRow(Function * function) = 0;
  virtual void editExpression(Function * function, Ion::Events::Event event) = 0;
  virtual ListParameterController * parameterController() = 0;
  virtual int maxNumberOfRows() = 0;
  virtual HighlightCell * titleCells(int index) = 0;
  virtual HighlightCell * expressionCells(int index) = 0;
  virtual void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) = 0;
  virtual void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) = 0;
  EvenOddCell * m_emptyCell;
  I18n::Message m_addNewMessage;
  NewFunctionCell * m_addNewFunction;
  Button m_plotButton;
  Button m_valuesButton;
};

}

#endif
