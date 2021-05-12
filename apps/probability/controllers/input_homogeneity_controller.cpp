#include "input_homogeneity_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <kandinsky/color.h>
#include <kandinsky/font.h>
#include <string.h>

using namespace Probability;

InputHomogeneityDataSource::InputHomogeneityDataSource(Responder * parentResponder, SelectableTableView * tableView,
                                                       InputEventHandlerDelegate * inputEventHandlerDelegate,
                                                       TextFieldDelegate * delegate)
    : m_topLeftCell(Escher::Palette::GrayMiddle) {
  // Headers
  char txt[20];
  char alphabet[] = "ABCDEFGHIJ";
  char numbers[] = "123456789";
  const char * groupTxt = I18n::translate(I18n::Message::Group);
  int offset = strlen(groupTxt);
  strcpy(txt, groupTxt);
  strcpy(txt + offset, " ");
  offset++;
  // First row
  for (int i = 0; i < k_initialNumberOfColumns; i++) {
    memcpy(txt + offset, &numbers[i], 1);
    m_colHeader[i].setText(txt);
    m_colHeader[i].setFont(KDFont::SmallFont);
  }
  // First column
  for (int i = 0; i < k_initialNumberOfRows; i++) {
    memcpy(txt + offset, &alphabet[i], 1);
    m_rowHeader[i].setText(txt);
    m_rowHeader[i].setFont(KDFont::SmallFont);
  }

  // EditableCells
  int numberOfCols = numberOfColumns();
  for (int i = 0; i < k_maxNumberOfEditableCells; i++) {
    m_cells[i].setParentResponder(tableView);
    m_cells[i].setEven(((i + 2) / (numberOfCols - 1)) % 2 == 0);
    m_cells[i].editableTextCell()->textField()->setDelegates(inputEventHandlerDelegate, delegate);
  }
}

HighlightCell * InputHomogeneityDataSource::reusableCell(int i, int type) {
  if (i == 0) {
    return &m_topLeftCell;
  }
  if (i < numberOfColumns()) {
    return &m_colHeader[i - 1];
  }
  if (i % numberOfColumns() == 0) {
    return &m_rowHeader[i / numberOfColumns() - 1];
  }
  int index = indexForEditableCell(i);
  return &m_cells[index];
}

int InputHomogeneityDataSource::indexForEditableCell(int i) {
  // Substracts the number of cells in top and left header before this index
  int row = i / numberOfColumns();
  return i - row - numberOfColumns() + 1;
}

InputHomogeneityController::InputHomogeneityController(StackViewController * parent,
                                                       HomogeneityResultsController * homogeneityResultsController,
                                                       InputEventHandlerDelegate * inputEventHandlerDelegate,
                                                       TextFieldDelegate * delegate)
    : Page(parent),
      m_data(this, &m_table, inputEventHandlerDelegate, delegate),
      m_contentView(this, this, &m_table, inputEventHandlerDelegate, delegate),
      m_table(&m_contentView, &m_data, m_contentView.selectionDataSource()),
      m_homogeneityResultsController(homogeneityResultsController) {}

void InputHomogeneityController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_contentView);
}

void InputHomogeneityController::buttonAction() {
  openPage(m_homogeneityResultsController, false);
}
