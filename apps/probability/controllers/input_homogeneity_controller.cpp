#include "input_homogeneity_controller.h"

#include <apps/i18n.h>
#include <kandinsky/color.h>
#include <string.h>

using namespace Probability;

InputHomogeneityDataSource::InputHomogeneityDataSource(Responder * parentResponder,
                                                       InputEventHandlerDelegate * inputEventHandlerDelegate,
                                                       TextFieldDelegate * delegate)
    : m_topLeftCell(KDColorBlack) {
  // Headers
  char txt[20];
  char alphabet[] = "abcdefghijklm";
  char numbers[] = "123456789";
  const char * groupTxt = I18n::translate(I18n::Message::Group);
  int offset = strlen(groupTxt);
  strcpy(txt, groupTxt);
  strcpy(txt + offset, " ");
  offset++;
  // First row
  for (int i = 0; i < k_initialNumberOfColumns; i++) {
    memcpy(txt + offset, &alphabet[i], 1);
    m_colHeader[i].setText(txt);
  }
  // First column
  for (int i = 0; i < k_initialNumberOfRows; i++) {
    memcpy(txt + offset, &numbers[i], 1);
    m_colHeader[i].setText(txt);
  }

  // EditableCells
  for (int i = 0; i < k_maxNumberOfEditableCells; i++) {
    m_cells[i].setParentResponder(parentResponder);
    m_cells[i].setEven((i / numberOfColumns()) % 2 == 0);
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
    return &m_rowHeader[i / numberOfColumns()];
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
                                                       InputEventHandlerDelegate * inputEventHandlerDelegate,
                                                       TextFieldDelegate * delegate)
    : Page(parent),
      m_data(this, inputEventHandlerDelegate, delegate),
      m_contentView(this, &m_table, inputEventHandlerDelegate, delegate),
      m_table(this, &m_data, m_contentView.selectionDataSource()) {}
