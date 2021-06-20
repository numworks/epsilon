#ifndef SHARED_LIST_PARAM_CONTROLLER_H
#define SHARED_LIST_PARAM_CONTROLLER_H

#include <escher.h>
#include "function_store.h"
#include "color_parameter_controller.h"
#include <apps/i18n.h>

namespace Shared {

class ListParameterController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate = nullptr);

  View * view() override { return &m_selectableTableView; }
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("ListParameter");
  void setRecord(Ion::Storage::Record record);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  int numberOfRows() const override { return totalNumberOfCells(); }

  // ListViewDataSource
  KDCoordinate rowHeight(int j) override { return Metric::ParameterCellHeight; }
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return 1; }
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
protected:
  virtual bool handleEnterOnRow(int rowIndex);
  virtual int totalNumberOfCells() const {
    return 3;
  }
  FunctionStore * functionStore();
  ExpiringPointer<Function> function();
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
private:
  MessageTableCellWithChevronAndMessage m_colorCell;
  MessageTableCellWithSwitch m_enableCell;
  MessageTableCell m_deleteCell;
  ColorParameterController m_colorParameterController;
};

}

#endif
