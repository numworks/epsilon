#include "program_parameter_controller.h"

namespace Code {

ProgramParameterController::ProgramParameterController(Responder * parentResponder, I18n::Message title, ProgramStore * programStore) :
  ViewController(parentResponder),
  m_pageTitle(title),
  m_editProgram(I18n::Message::Default),
  m_renameProgram(I18n::Message::Default),
  m_autoImportProgram(I18n::Message::Default),
  m_deleteProgram(I18n::Message::Default),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_programStore(programStore)
{
}

void ProgramParameterController::setProgram(int i){
  m_editorController.setProgram(m_programStore->editableProgram(i));
}

View * ProgramParameterController::view() {
  return &m_selectableTableView;
}

const char * ProgramParameterController::title() {
  return I18n::translate(m_pageTitle);
}

bool ProgramParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 1)) {
    switch (selectedRow()) {
      case 0:
        app()->displayModalViewController(&m_editorController, 0.5f, 0.5f);
        return true;
      //TODO other cases
      default:
        assert(false);
        return false;
    }
  }
  return false;
}

void ProgramParameterController::didBecomeFirstResponder() {
 if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

KDCoordinate ProgramParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

HighlightCell * ProgramParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_editProgram, &m_renameProgram, &m_autoImportProgram, &m_deleteProgram};
  return cells[index];

}

int ProgramParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

int ProgramParameterController::numberOfRows() {
 return k_totalNumberOfCell;
}

void ProgramParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCell * myCell = (MessageTableCell *)cell;
  I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::EditScript, I18n::Message::RenameScript, I18n::Message::AutoImportScript, I18n::Message::DeleteScript};
  myCell->setMessage(labels[index]);
}

}
