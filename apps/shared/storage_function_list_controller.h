#ifndef SHARED_STORAGE_FUNCTION_LIST_CONTROLLER_H
#define SHARED_STORAGE_FUNCTION_LIST_CONTROLLER_H

#include <escher.h>
#include "function_title_cell.h"
#include "storage_function_store.h"
#include "function_app.h"
#include "storage_list_parameter_controller.h"
#include "storage_expression_model_list_controller.h"
#include "../i18n.h"

namespace Shared {

template<class T>
class StorageFunctionListController : public StorageExpressionModelListController<T>, public ButtonRowDelegate, public TableViewDataSource, public SelectableTableViewDelegate {
public:
  StorageFunctionListController(Responder * parentResponder, StorageFunctionStore<T> * functionStore, ButtonRowController * header, ButtonRowController * footer, I18n::Message text) :
    StorageExpressionModelListController<T>(parentResponder, text),
    ButtonRowDelegate(header, footer),
    m_functionStore(functionStore),
    m_selectableTableView(this, this, this, this),
    m_emptyCell(),
    m_plotButton(this, I18n::Message::Plot, Invocation([](void * context, void * sender) {
          StorageFunctionListController * list = (StorageFunctionListController *)context;
          TabViewController * tabController = list->tabController();
          tabController->setActiveTab(1);
          }, this), KDText::FontSize::Small, Palette::PurpleBright),
    m_valuesButton(this, I18n::Message::DisplayValues, Invocation([](void * context, void * sender) {
          StorageFunctionListController * list = (StorageFunctionListController *)context;
          TabViewController * tabController = list->tabController();
          tabController->setActiveTab(2);
          }, this), KDText::FontSize::Small, Palette::PurpleBright),
    m_titlesColumnWidth(k_minTitleColumnWidth)
  {
    m_selectableTableView.setMargins(0);
    m_selectableTableView.setVerticalCellOverlap(0);
    m_selectableTableView.setShowsIndicators(false);
  }

  /* TableViewDataSource */
  int numberOfRows() override {
    return this->numberOfExpressionRows();
  }
  int numberOfColumns() override { return 2; }
  KDCoordinate rowHeight(int j) override {
    return this->expressionRowHeight(j);
  }

  void viewWillAppear() override {
    computeTitlesColumnWidth();
  }

  void computeTitlesColumnWidth() {
    KDCoordinate maxTitleWidth = maxFunctionNameWidth()+k_functionTitleSumOfMargins;
    m_titlesColumnWidth = maxTitleWidth < k_minTitleColumnWidth ? k_minTitleColumnWidth : maxTitleWidth;
  }

  KDCoordinate columnWidth(int i) override {
    switch (i) {
      case 0:
        return m_titlesColumnWidth;
      case 1:
        return selectableTableView()->bounds().width()-m_titlesColumnWidth;
      default:
        assert(false);
        return 0;
    }
  }

  KDCoordinate cumulatedWidthFromIndex(int i) override {
    switch (i) {
      case 0:
        return 0;
      case 1:
        return m_titlesColumnWidth;
      case 2:
        return selectableTableView()->bounds().width();
      default:
        assert(false);
        return 0;
    }
  }
  int indexFromCumulatedWidth(KDCoordinate offsetX) override {
    if (offsetX <= m_titlesColumnWidth) {
      return 0;
    } else {
      if (offsetX <= selectableTableView()->bounds().width())
        return 1;
      else {
        return 2;
      }
    }
  }
  int typeAtLocation(int i, int j) override {
    if (this->isAddEmptyRow(j)){
      return i + 2;
    }
    return i;
  }

  HighlightCell * reusableCell(int index, int type) override {
    assert(index >= 0);
    assert(index < maxNumberOfDisplayableRows());
    switch (type) {
      case 0:
        return titleCells(index);
      case 1:
        return expressionCells(index);
      case 2:
        return &m_emptyCell;
      case 3:
        return &(this->m_addNewModel);
      default:
        assert(false);
        return nullptr;
    }
  }
  int reusableCellCount(int type) override {
    if (type > 1) {
      return 1;
    }
    return maxNumberOfDisplayableRows();
  }

  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override {
    if (!this->isAddEmptyRow(j)) {
      if (i == 0) {
        willDisplayTitleCellAtIndex(cell, j);
      } else {
        this->willDisplayExpressionCellAtIndex(cell, j);
      }
    }
    EvenOddCell * myCell = (EvenOddCell *)cell;
    myCell->setEven(j%2 == 0);
    myCell->setHighlighted(i == this->selectedColumn() && j == this->selectedRow());
    myCell->reloadCell();
  }
  /* ButtonRowDelegate */
  int numberOfButtons(ButtonRowController::Position position) const override {
    if (position == ButtonRowController::Position::Bottom) {
      return 2;
    }
    return 0;
  }
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override {
    if (position == ButtonRowController::Position::Top) {
      return nullptr;
    }
    const Button * buttons[2] = {&m_plotButton, &m_valuesButton};
    return (Button *)buttons[index];
  }
  /* Responder */
  void didBecomeFirstResponder() override {
    if (this->selectedRow() == -1) {
      this->selectCellAtLocation(1, 0);
    } else {
      this->selectCellAtLocation(this->selectedColumn(), this->selectedRow());
    }
    if (this->selectedRow() >= this->numberOfRows()) {
      this->selectCellAtLocation(this->selectedColumn(), this->numberOfRows()-1);
    }
    footer()->setSelectedButton(-1);
    this->app()->setFirstResponder(selectableTableView());
  }
  bool handleEvent(Ion::Events::Event event) override {
    if (event == Ion::Events::Up) {
      if (this->selectedRow() == -1) {
        footer()->setSelectedButton(-1);
        selectableTableView()->selectCellAtLocation(1, numberOfRows()-1);
        this->app()->setFirstResponder(selectableTableView());
        return true;
      }
      selectableTableView()->deselectTable();
      assert(this->selectedRow() == -1);
      this->app()->setFirstResponder(tabController());
      return true;
    }
    if (event == Ion::Events::Down) {
      if (this->selectedRow() == -1) {
        return false;
      }
      selectableTableView()->deselectTable();
      footer()->setSelectedButton(0);
      return true;
    }
    if (this->selectedRow() < 0) {
      return false;
    }
    if (this->selectedColumn() == 1) {
      return this->handleEventOnExpression(event);
    }
    if (event == Ion::Events::OK || event == Ion::Events::EXE) {
      assert(this->selectedColumn() == 0);
      T func = m_functionStore->modelAtIndex(this->modelIndexForRow(this->selectedRow()));
      configureFunction(&func);
      return true;
    }
    if (event == Ion::Events::Backspace) {
      T function = m_functionStore->modelAtIndex(this->modelIndexForRow(this->selectedRow()));
      if (this->removeModelRow(&function)) {
        int newSelectedRow = this->selectedRow() >= this->numberOfRows() ? this->numberOfRows()-1 : this->selectedRow();
        this->selectCellAtLocation(this->selectedColumn(), newSelectedRow);
        selectableTableView()->reloadData();
      }
      return true;
    }
    return false;
  }
  void didEnterResponderChain(Responder * previousFirstResponder) override {
    selectableTableView()->reloadData();
  }
  void willExitResponderChain(Responder * nextFirstResponder) override {
    if (nextFirstResponder == tabController()) {
      selectableTableView()->deselectTable();
      footer()->setSelectedButton(-1);
    }
  }
  /* ViewController */
  View * view() override { return &m_selectableTableView; }
  /* SelectableTableViewDelegate*/
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override {
    if (this->isAddEmptyRow(this->selectedRow()) && this->selectedColumn() == 0) {
      t->selectCellAtLocation(1, numberOfRows()-1);
    }

  }
  /* ExpressionModelListController */
  SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
protected:
  StackViewController * stackController() const {
    return (StackViewController *)(this->parentResponder()->parentResponder()->parentResponder());
  }
  void configureFunction(T * function) {
    StackViewController * stack = stackController();
    parameterController()->setFunction(function);
    stack->push(parameterController());
  }
  StorageFunctionStore<T> * m_functionStore;
private:
  static constexpr KDCoordinate k_minTitleColumnWidth = 65;
  static constexpr KDCoordinate k_functionTitleSumOfMargins = 2*Metric::HistoryHorizontalMargin;
  TabViewController * tabController() const {
    return (TabViewController *)(this->parentResponder()->parentResponder()->parentResponder()->parentResponder());
  }
  StorageExpressionModelStore<T> * modelStore() override { return m_functionStore; }
  InputViewController * inputController() override {
    FunctionApp * myApp = static_cast<FunctionApp *>(this->app());
    return myApp->inputViewController();
  }
  virtual StorageListParameterController<T> * parameterController() = 0;
  virtual int maxNumberOfDisplayableRows() = 0;
  virtual FunctionTitleCell * titleCells(int index) = 0;
  virtual HighlightCell * expressionCells(int index) = 0;
  virtual void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) = 0;
  virtual KDCoordinate maxFunctionNameWidth() const = 0;
  SelectableTableView m_selectableTableView;
  EvenOddCell m_emptyCell;
  Button m_plotButton;
  Button m_valuesButton;
  KDCoordinate m_titlesColumnWidth;
};

}

#endif
