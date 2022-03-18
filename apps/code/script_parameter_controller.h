#ifndef CODE_SCRIPT_PARAMETER_CONTROLLER_H
#define CODE_SCRIPT_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_message_with_switch.h>
#include <escher/stack_view_controller.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_list_view_controller.h>
#include "script_store.h"

namespace Code {

class MenuController;

class ScriptParameterController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource> {
public:
  ScriptParameterController(Escher::Responder * parentResponder, I18n::Message title, MenuController * menuController);
  void setScript(Script script);
  void dismissScriptParameterController();

  /* ViewController */
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("ScriptParameter");

  /* MemoizedListViewDataSource */
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int numberOfRows() const override { return k_totalNumberOfCell; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

private:
  constexpr static int k_totalNumberOfCell = 4;
  Escher::StackViewController * stackViewController();
  I18n::Message m_pageTitle;
  Escher::MessageTableCell m_executeScript;
  Escher::MessageTableCell m_renameScript;
  Escher::MessageTableCellWithMessageWithSwitch m_autoImportScript;
  Escher::MessageTableCell m_deleteScript;
  Script m_script;
  MenuController * m_menuController;
};

}

#endif
