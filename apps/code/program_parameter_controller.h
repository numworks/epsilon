#ifndef CODE_PROGRAM_PARAMETER_CONTROLLER_H
#define CODE_PROGRAM_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../i18n.h"
#include "editor_controller.h"
#include "program_store.h"

namespace Code {

class ProgramParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  ProgramParameterController(Responder * parentResponder, I18n::Message title, ProgramStore * programStore);
  void setProgram(int i);

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
  MessageTableCell m_editProgram;
  MessageTableCell m_renameProgram;
  MessageTableCellWithSwitch m_autoImportProgram;
  MessageTableCell m_deleteProgram;
  SelectableTableView m_selectableTableView;
  EditorController m_editorController;
  ProgramStore * m_programStore;
};

}

#endif
