#ifndef CODE_SCRIPT_PARAMETER_CONTROLLER_H
#define CODE_SCRIPT_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../i18n.h"
#include "script_store.h"

namespace Code {

class MenuController;

class ScriptParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  ScriptParameterController(Responder * parentResponder, I18n::Message title, ScriptStore * scriptStore, MenuController * menuController);
  void setScript(int i);
  void dismissScriptParameterController();

  /* ViewController */
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;

  /* SimpleListViewDataSource */
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  int numberOfRows() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

private:
  constexpr static int k_totalNumberOfCell = 4;
  StackViewController * stackViewController();
  I18n::Message m_pageTitle;
  MessageTableCell m_editScript; //TODO rename execute script
  MessageTableCell m_renameScript;
  MessageTableCellWithSwitch m_autoImportScript;
  MessageTableCell m_deleteScript;
  SelectableTableView m_selectableTableView;
  ScriptStore * m_scriptStore;
  MenuController * m_menuController;
  bool m_autoImport;
  int m_currentScriptIndex;
};

}

#endif
