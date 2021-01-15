#ifndef CODE_SCRIPT_PARAMETER_CONTROLLER_H
#define CODE_SCRIPT_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_switch.h>
#include <escher/stack_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/simple_list_view_data_source.h>
#include "script_store.h"

namespace Code {

class MenuController;

class ScriptParameterController : public Escher::ViewController, public Escher::ListViewDataSource, public Escher::SelectableTableViewDataSource {
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

  /* ListViewDataSource */
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int numberOfRows() const override { return k_totalNumberOfCell; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  KDCoordinate cellWidth() override {
    assert(m_selectableTableView.columnWidth(0) > 0);
    return m_selectableTableView.columnWidth(0);
  }

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
