#include "sequence_title_cell.h"

using namespace Shared;

namespace Sequence {

SequenceTitleCell::SequenceTitleCell(Responder * parentResponder, ListParameterController * listParameterController) :
  SequenceCell(parentResponder),
  m_backgroungCell(FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator)),
  m_definitionView(KDText::FontSize::Large, 0.5f, 0.5f),
  m_firstInitialConditionView(KDText::FontSize::Large, 0.5f, 0.5f),
  m_secondInitialConditionView(KDText::FontSize::Large, 0.5f, 0.5f),
  m_listParameterController(listParameterController)
{
}

void SequenceTitleCell::setSequence(Sequence * sequence) {
  SequenceCell::setSequence(sequence);
  char bufferName[5] = {*sequence->name(),'(',sequence->symbol(),')', 0};
  m_definitionView.setText(bufferName);
  if (m_numberOfSubCells > 0) {
    char bufferName[7] = {*sequence->name(),'(',sequence->symbol(),'+','1',')', 0};
    m_firstInitialConditionView.setText(bufferName);
  }
  if (m_numberOfSubCells > 1) {
    char bufferName[7] = {*sequence->name(),'(',sequence->symbol(),'+','2',')', 0};
    m_secondInitialConditionView.setText(bufferName);
  }
  KDColor nameColor = sequence->isActive() ? sequence->color() : Palette::GreyDark;
  setColor(nameColor);
  layoutSubviews();
}

void SequenceTitleCell::setColor(KDColor color) {
  m_backgroungCell.setColor(color);
  m_definitionView.setTextColor(color);
  m_firstInitialConditionView.setTextColor(color);
  m_secondInitialConditionView.setTextColor(color);
}

int SequenceTitleCell::numberOfSubviews() const {
  return SequenceCell::numberOfSubviews() + 1;
}

View * SequenceTitleCell::subviewAtIndex(int index) {
  if (index == SequenceCell::numberOfSubviews()) {
    return &m_backgroungCell;
  }
  return SequenceCell::subviewAtIndex(index);
}

void SequenceTitleCell::layoutSubviews() {
  m_backgroungCell.setFrame(bounds());
  SequenceCell::layoutSubviews();
}

void SequenceTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor separatorColor = m_even ? Palette::WallScreen : KDColorWhite;
  KDCoordinate cellHeight = (bounds().height()-(m_numberOfSubCells-1)*k_separatorThickness)/m_numberOfSubCells;
  if (m_numberOfSubCells > 1) {
    ctx->fillRect(KDRect(0, cellHeight, bounds().width(), k_separatorThickness), separatorColor);
  }
  if (m_numberOfSubCells > 2) {
    ctx->fillRect(KDRect(0, 2*cellHeight+k_separatorThickness, bounds().width(), k_separatorThickness), separatorColor);
  }
}

bool SequenceTitleCell::handleEvent(Ion::Events::Event event) {
  if (m_selectedSubCell < m_numberOfSubCells-1 && event == Ion::Events::Down) {
    selectSubCell(m_selectedSubCell+1);
    return true;
  }
  if (m_selectedSubCell > 0 && event == Ion::Events::Up) {
    selectSubCell(m_selectedSubCell-1);
    return true;
  }
  if (event == Ion::Events::OK) {
    StackViewController * stack = stackController();
    m_listParameterController->setSequence(m_sequence);
    stack->push(m_listParameterController);
    SelectableTableView * table = (SelectableTableView *)parentResponder();
    table->dataHasChanged(true);
    return true;
  }
  return false;
}

EvenOddCell * SequenceTitleCell::viewAtIndex(int index) {
  if (index == 0) {
    return &m_definitionView;
  }
  if (index == 1) {
    return &m_firstInitialConditionView;
  }
  return &m_secondInitialConditionView;
}

StackViewController * SequenceTitleCell::stackController() {
  return (StackViewController *)parentResponder()->parentResponder()->parentResponder()->parentResponder();
}

}
