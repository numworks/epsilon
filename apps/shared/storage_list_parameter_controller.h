#ifndef SHARED_STORAGE_LIST_PARAM_CONTROLLER_H
#define SHARED_STORAGE_LIST_PARAM_CONTROLLER_H

#include <escher.h>
#include "storage_function_store.h"
#include <apps/i18n.h>

namespace Shared {

class StorageListParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  StorageListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate = nullptr);

  View * view() override { return &m_selectableTableView; }
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  virtual void setRecord(Ion::Storage::Record record);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  int numberOfRows() override { return totalNumberOfCells(); }
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override { return totalNumberOfCells(); }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
protected:
  virtual bool handleEnterOnRow(int rowIndex);
  virtual int totalNumberOfCells() const {
#if FUNCTION_COLOR_CHOICE
    return 3;
#else
    return 2;
#endif
  }
  FunctionStore * functionStore();
  ExpiringPointer<Function> function();
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
private:
#if FUNCTION_COLOR_CHOICE
  MessageTableCellWithChevron m_colorCell;
#endif
  MessageTableCellWithSwitch m_enableCell;
  MessageTableCell m_deleteCell;
};

}

#endif
