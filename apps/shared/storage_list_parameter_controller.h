#ifndef SHARED_STORAGE_LIST_PARAM_CONTROLLER_H
#define SHARED_STORAGE_LIST_PARAM_CONTROLLER_H

#include <escher.h>
#include "storage_function.h"
#include "storage_function_store.h"
#include "../i18n.h"

namespace Shared {

template <class T>
class StorageListParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  StorageListParameterController(Responder * parentResponder, StorageFunctionStore<T> * functionStore, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate = nullptr) :
    ViewController(parentResponder),
    m_selectableTableView(this, this, this, tableDelegate),
    m_functionStore(functionStore),
    m_function(nullptr),
#if FUNCTION_COLOR_CHOICE
    m_colorCell(functionColorMessage),
#endif
    m_enableCell(I18n::Message::ActivateDesactivate),
    m_deleteCell(deleteFunctionMessage)
  {}

  View * view() override {
    return &m_selectableTableView;
  }
  const char * title() override {
    return I18n::translate(I18n::Message::FunctionOptions);
  }
  bool handleEvent(Ion::Events::Event event) override {
    if (event == Ion::Events::OK || event == Ion::Events::EXE) {
      return handleEnterOnRow(selectedRow());
    }
    return false;
  }
  virtual void setFunction(T * function) {
    m_function = function;
    selectCellAtLocation(0, 0);
  }
  void didBecomeFirstResponder() override {
    app()->setFirstResponder(&m_selectableTableView);
  }
  void viewWillAppear() override {
    ViewController::viewWillAppear();
    if (selectedRow() == -1) {
      selectCellAtLocation(0, 0);
    } else {
      selectCellAtLocation(selectedColumn(), selectedRow());
    }
    m_selectableTableView.reloadData();
  }
  int numberOfRows() override {
    return k_totalNumberOfCell;
  }
  KDCoordinate cellHeight() override {
    return Metric::ParameterCellHeight;
  }
  HighlightCell * reusableCell(int index) override {
    assert(index >= 0);
    assert(index < k_totalNumberOfCell);
#if FUNCTION_COLOR_CHOICE
    HighlightCell * cells[] = {&m_colorCell, &m_enableCell, &m_deleteCell};
#else
    HighlightCell * cells[] = {&m_enableCell, &m_deleteCell};
#endif
    return cells[index];
  }
  int reusableCellCount() override {
    return k_totalNumberOfCell;
  }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override {
    if (cell == &m_enableCell) {
      SwitchView * switchView = (SwitchView *)m_enableCell.accessoryView();
      switchView->setState(m_function->isActive());
    }
  }
protected:
  bool handleEnterOnRow(int rowIndex) {
    switch (rowIndex) {
#if FUNCTION_COLOR_CHOICE
      case 0:
        /* TODO: implement function color choice */
        return true;
      case 1:
#else
      case 0:
#endif
        m_function->setActive(!m_function->isActive());
        m_selectableTableView.reloadData();
        return true;
#if FUNCTION_COLOR_CHOICE
      case 2:
#else
      case 1:
#endif
        {
          if (m_functionStore->numberOfModels() > 1) {
            m_functionStore->removeModel(*m_function);
            StackViewController * stack = (StackViewController *)(parentResponder());
            stack->pop();
            return true;
          } else {
            if (m_functionStore->numberOfDefinedModels() == 1) {
              T f = m_functionStore->definedFunctionAtIndex(0);
              f.setContent("");
              StackViewController * stack = (StackViewController *)(parentResponder());
              stack->pop();
              return true;
            }
            app()->displayWarning(I18n::Message::NoFunctionToDelete);
            return true;
          }
        }
      default:
        return false;
    }
  }
  SelectableTableView m_selectableTableView;
  StorageFunctionStore<T> * m_functionStore;
  T * m_function;
private:
#if FUNCTION_COLOR_CHOICE
  constexpr static int k_totalNumberOfCell = 3;
  MessageTableCellWithChevron m_colorCell;
#else
  constexpr static int k_totalNumberOfCell = 2;
#endif
  MessageTableCellWithSwitch m_enableCell;
  MessageTableCell m_deleteCell;
};

}

#endif
