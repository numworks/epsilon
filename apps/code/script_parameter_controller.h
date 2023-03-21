#ifndef CODE_SCRIPT_PARAMETER_CONTROLLER_H
#define CODE_SCRIPT_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/stack_view_controller.h>
#include <escher/switch_view.h>

#include "script_store.h"

namespace Code {

class MenuController;

class ScriptParameterController : public Escher::SelectableListViewController<
                                      Escher::MemoizedListViewDataSource> {
 public:
  ScriptParameterController(Escher::Responder* parentResponder,
                            I18n::Message title,
                            MenuController* menuController);
  void setScript(Script script);
  void dismissScriptParameterController();

  /* ViewController */
  const char* title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("ScriptParameter");

  /* MemoizedListViewDataSource */
  Escher::AbstractMenuCell* reusableCell(int index, int type) override;
  int numberOfRows() const override { return k_totalNumberOfCell; }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 private:
  constexpr static int k_totalNumberOfCell = 4;
  Escher::StackViewController* stackViewController();
  I18n::Message m_pageTitle;
  Escher::MenuCell<Escher::MessageTextView> m_executeScript;
  Escher::MenuCell<Escher::MessageTextView> m_renameScript;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::SwitchView>
      m_autoImportScript;
  Escher::MenuCell<Escher::MessageTextView> m_deleteScript;
  Script m_script;
  MenuController* m_menuController;
};

}  // namespace Code

#endif
