#ifndef CODE_SCRIPT_PARAMETER_CONTROLLER_H
#define CODE_SCRIPT_PARAMETER_CONTROLLER_H

#include <escher.h>
#include <apps/i18n.h>
#include "script_store.h"

namespace Code {

class MenuController;

class ScriptParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  ScriptParameterController(Responder * parentResponder, I18n::Message title, MenuController * menuController);
  void setScript(Script script);
  void dismissScriptParameterController();

  /* ViewController */
  View * view() override { return &m_selectableTableView; }
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("ScriptParameter");

  /* SimpleListViewDataSource */
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override { return k_totalNumberOfCell; }
  int numberOfRows() const override { return k_totalNumberOfCell; }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

private:
  constexpr static int k_totalNumberOfCell = 6;
  StackViewController * stackViewController();
  I18n::Message m_pageTitle;
  MessageTableCell<> m_executeScript;
  MessageTableCell<> m_renameScript;
  MessageTableCellWithSwitch m_autoImportScript;
  MessageTableCell<> m_deleteScript;
  MessageTableCell<> m_duplicateScript;
  MessageTableCellWithBuffer m_size;
  void GetScriptSize(MessageTableCellWithBuffer* myCell);
  SelectableTableView m_selectableTableView;
  Script m_script;
  MenuController * m_menuController;
  bool m_sizedisplaypercent = false;
};

}

#endif
