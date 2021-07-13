#include "input_homogeneity_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <kandinsky/color.h>
#include <kandinsky/font.h>
#include <string.h>

#include "probability/app.h"
#include "probability/models/data.h"

using namespace Probability;

InputHomogeneityDataSource::InputHomogeneityDataSource(
    SelectableTableView * tableView,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityStatistic * statistic) :
    m_statistic(statistic), m_table(tableView) {
  int numberOfCols = numberOfColumns();
  for (int i = 0; i < HomogeneityTableDataSource::k_maxNumberOfInnerCells; i++) {
    m_cells[i].setParentResponder(tableView);
    m_cells[i].setEven(((i + 2) / numberOfCols) % 2 == 1);
    m_cells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, this);
    m_cells[i].setFont(KDFont::SmallFont);
  }
}

HighlightCell * InputHomogeneityDataSource::reusableCell(int i, int type) {
  return &m_cells[i];
}

bool Probability::InputHomogeneityDataSource::textFieldShouldFinishEditing(
    Escher::TextField * textField,
    Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool Probability::InputHomogeneityDataSource::textFieldDidFinishEditing(
    Escher::TextField * textField,
    const char * text,
    Ion::Events::Event event) {
  float p;
  if (textFieldDelegateApp()->hasUndefinedValue(text, p, false, false)) {
    return false;
  }

  m_statistic->setParameterAtPosition(m_table->selectedRow() - 1, m_table->selectedColumn() - 1, p);
  if (m_table->selectedRow() == numberOfRows() - 1 &&
      numberOfRows() < HomogeneityTableDataSource::k_maxNumberOfRows) {
    // TODO add row
  }
  m_table->selectCellAtLocation(m_table->selectedColumn(), m_table->selectedRow() + 1);
  m_table->reloadData(false);
  return true;
}

void Probability::InputHomogeneityDataSource::willDisplayCellAtLocation(
    Escher::HighlightCell * cell,
    int column,
    int row) {
}

InputHomogeneityController::InputHomogeneityController(
    StackViewController * parent,
    HomogeneityResultsController * homogeneityResultsController,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityStatistic * statistic) :
    Page(parent),
    m_innerTableData(&m_table, inputEventHandlerDelegate, statistic),
    m_tableData(&m_innerTableData),
    m_table(&m_contentView, &m_tableData, m_contentView.selectionDataSource(), &m_tableData),
    m_contentView(this, this, &m_table, inputEventHandlerDelegate, this),
    m_homogeneityResultsController(homogeneityResultsController),
    m_statistic(statistic) {
}

void InputHomogeneityController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::InputHomogeneity);
  Container::activeApp()->setFirstResponder(&m_contentView);
}

void InputHomogeneityController::buttonAction() {
  openPage(m_homogeneityResultsController);
}

bool Probability::InputHomogeneityController::textFieldShouldFinishEditing(
    TextField * textField,
    Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;  // TODO up and down too
}

bool Probability::InputHomogeneityController::textFieldDidFinishEditing(TextField * textField,
                                                                        const char * text,
                                                                        Ion::Events::Event event) {
  // TODO parse significance cell
}
