#include "input_homogeneity_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <kandinsky/color.h>
#include <kandinsky/font.h>
#include <string.h>

#include "probability/app.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"

using namespace Probability;

InputHomogeneityDataSource::InputHomogeneityDataSource(
    SelectableTableView * tableView,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityStatistic * statistic) :
    m_numberOfRows(HomogeneityTableDataSource::k_initialNumberOfRows),
    m_numberOfColumns(HomogeneityTableDataSource::k_initialNumberOfColumns),
    m_statistic(statistic),
    m_table(tableView) {
  for (int i = 0; i < HomogeneityTableDataSource::k_maxNumberOfInnerCells; i++) {
    m_cells[i].setParentResponder(tableView);
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
  if (m_table->selectedRow() == numberOfRows() &&
      numberOfRows() < HomogeneityTableDataSource::k_maxNumberOfRows) {
    m_numberOfRows++;
  }
  if (m_table->selectedColumn() == numberOfColumns() &&
      numberOfColumns() < HomogeneityTableDataSource::k_maxNumberOfColumns) {
    m_numberOfColumns++;
  }
  m_table->selectCellAtLocation(m_table->selectedColumn(), m_table->selectedRow() + 1);
  m_table->reloadData(false);
  return true;
}

void Probability::InputHomogeneityDataSource::willDisplayCellAtLocation(
    Escher::HighlightCell * cell,
    int column,
    int row) {
  float p = m_statistic->parameterAtPosition(row, column);
  Escher::EvenOddEditableTextCell * myCell = static_cast<Escher::EvenOddEditableTextCell *>(cell);
  if (std::isnan(p)) {
    myCell->editableTextCell()->textField()->setText("");
  } else {
    constexpr int bufferSize = 20;
    char buffer[bufferSize];
    defaultParseFloat(p, buffer, bufferSize);
    myCell->editableTextCell()->textField()->setText(buffer);
  }
  myCell->setEven(row % 2 == 0);
}

InputHomogeneityController::InputHomogeneityController(
    StackViewController * parent,
    HomogeneityResultsController * homogeneityResultsController,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityStatistic * statistic) :
    Page(parent),
    ChainedSelectableTableViewDelegate(&m_tableData),
    m_innerTableData(&m_table, inputEventHandlerDelegate, statistic),
    m_tableData(&m_innerTableData),
    m_table(&m_contentView, &m_tableData, m_contentView.selectionDataSource(), this),
    m_contentView(this, this, &m_table, inputEventHandlerDelegate, this),
    m_statistic(statistic),
    m_homogeneityResultsController(homogeneityResultsController) {
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
  return false;
}

void Probability::InputHomogeneityController::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  ChainedSelectableTableViewDelegate::tableViewDidChangeSelectionAndDidScroll(
      t,
      previousSelectedCellX,
      previousSelectedCellY,
      withinTemporarySelection);
  // TODO factor with InputGoodnessController
  int row = m_table.selectedRow();
  int col = m_table.selectedColumn();
  if (!withinTemporarySelection && previousSelectedCellY != row) {
    // Scroll to cell
    KDRect cellFrame = KDRect(
        0,
        m_tableData.cumulatedHeightFromIndex(row),
        m_tableData.columnWidth(col),
        m_tableData.rowHeight(row));  // TODO query m_inputTableView::cellFrame

    m_contentView.scrollToContentRect(cellFrame);

    m_contentView.layoutSubviews(true);
  }
}
