#ifndef CODE_SCRIPT_PARAMETER_CONTROLLER_H
#define CODE_SCRIPT_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../i18n.h"
#include "editor_controller.h"
#include "script_store.h"

namespace Code {

class ScriptParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  ScriptParameterController(Responder * parentResponder, I18n::Message title, ScriptStore * scriptStore);
  void setScript(int i);

  /* ViewController */
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  /* SimpleListViewDataSource */
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  int numberOfRows() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

private:
  constexpr static int k_totalNumberOfCell = 4;
  I18n::Message m_pageTitle;
  MessageTableCell m_editScript;
  MessageTableCell m_renameScript;
  MessageTableCellWithSwitch m_autoImportScript;
  MessageTableCell m_deleteScript;
  SelectableTableView m_selectableTableView;
  EditorController m_editorController;
  ScriptStore * m_scriptStore;
};

}

#endif
