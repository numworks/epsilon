#ifndef SHARED_FUNCTION_LIST_CONTROLLER_H
#define SHARED_FUNCTION_LIST_CONTROLLER_H

#include <escher.h>
#include "function_store.h"
#include "function_app.h"
#include "list_parameter_controller.h"
#include "expression_model_list_controller.h"
#include "../i18n.h"

namespace Shared {

class FunctionListController : public ExpressionModelListController, public ButtonRowDelegate, public TableViewDataSource, public SelectableTableViewDelegate {
public:
  FunctionListController(Responder * parentResponder, FunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer, I18n::Message text);

  /* TableViewDataSource */
  int numberOfRows() override {
    return numberOfExpressionRows();
  }
  int numberOfColumns() override;
  KDCoordinate rowHeight(int j) override {
    return expressionRowHeight(j);
  }
  KDCoordinate columnWidth(int i) override;
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int typeAtLocation(int i, int j) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  /* ButtonRowDelegate */
  int numberOfButtons(ButtonRowController::Position position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;
  /* Responder */
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  /* SelectableTableViewDelegate*/
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
protected:
  StackViewController * stackController() const;
  void configureFunction(Function * function);
  View * loadView() override;
  void unloadView(View * view) override;
  FunctionStore * m_functionStore;
private:
  static constexpr KDCoordinate k_functionNameWidth = 65;
  TabViewController * tabController() const;
  ExpressionModelStore * modelStore() override { return m_functionStore; }
  InputViewController * inputController() override {
    FunctionApp * myApp = static_cast<FunctionApp *>(app());
    return myApp->inputViewController();
  }
  virtual ListParameterController * parameterController() = 0;
  virtual int maxNumberOfRows() = 0;
  virtual HighlightCell * titleCells(int index) = 0;
  virtual HighlightCell * expressionCells(int index) = 0;
  virtual void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) = 0;
  EvenOddCell * m_emptyCell;
  Button m_plotButton;
  Button m_valuesButton;
};

}

#endif
