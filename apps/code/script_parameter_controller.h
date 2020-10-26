#ifndef CODE_SCRIPT_PARAMETER_CONTROLLER_H
#define CODE_SCRIPT_PARAMETER_CONTROLLER_H

#include <escher.h>
#include <apps/i18n.h>
#include "script_store.h"

namespace Code {

class MenuController;

class ScriptParameterController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  ScriptParameterController(Escher::Responder * parentResponder, I18n::Message title, MenuController * menuController);
  void setScript(Script script);
  void dismissScriptParameterController();

  /* ViewController */
  Escher::View * view() override { return &m_selectableTableView; }
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("ScriptParameter");

  /* SimpleListViewDataSource */
  KDCoordinate cellHeight() override { return Escher::Metric::ParameterCellHeight; }
  Escher::HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override { return k_totalNumberOfCell; }
  int numberOfRows() const override { return k_totalNumberOfCell; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

private:
  constexpr static int k_totalNumberOfCell = 4;
  Escher::StackViewController * stackViewController();
  I18n::Message m_pageTitle;
  Escher::MessageTableCell m_executeScript;
  Escher::MessageTableCell m_renameScript;
  Escher::MessageTableCellWithSwitch m_autoImportScript;
  Escher::MessageTableCell m_deleteScript;
  Escher::SelectableTableView m_selectableTableView;
  Script m_script;
  MenuController * m_menuController;
};

}

#endif
