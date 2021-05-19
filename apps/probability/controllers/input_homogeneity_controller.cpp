#include "input_homogeneity_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <kandinsky/color.h>
#include <kandinsky/font.h>
#include <string.h>

#include "probability/app.h"
#include "probability/data.h"

using namespace Probability;

InputHomogeneityDataSource::InputHomogeneityDataSource(
    SelectableTableView * tableView, InputEventHandlerDelegate * inputEventHandlerDelegate,
    TextFieldDelegate * delegate) {
  int numberOfCols = numberOfColumns();
  for (int i = 0; i < HomogeneityTableDataSource::k_maxNumberOfInnerCells; i++) {
    m_cells[i].setParentResponder(tableView);
    m_cells[i].setEven(((i + 2) / (numberOfCols - 1)) % 2 == 0);
    m_cells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, delegate);
  }
}

HighlightCell * InputHomogeneityDataSource::reusableCell(int i, int type) { return &m_cells[i]; }

InputHomogeneityController::InputHomogeneityController(
    StackViewController * parent, HomogeneityResultsController * homogeneityResultsController,
    InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate)
    : Page(parent),
      m_innerTableData(&m_table, inputEventHandlerDelegate, delegate),
      m_tableData(&m_innerTableData),
      m_contentView(this, this, &m_table, inputEventHandlerDelegate, delegate),
      m_table(&m_contentView, &m_tableData, m_contentView.selectionDataSource()),
      m_homogeneityResultsController(homogeneityResultsController) {}

void InputHomogeneityController::didBecomeFirstResponder() {
  Probability::App::app()->snapshot()->navigation()->setPage(Data::Page::InputHomogeneity);
  Container::activeApp()->setFirstResponder(&m_contentView);
}

void InputHomogeneityController::buttonAction() { openPage(m_homogeneityResultsController, false); }
