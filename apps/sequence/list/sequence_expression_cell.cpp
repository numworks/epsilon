#include "sequence_expression_cell.h"

using namespace Shared;

namespace Sequence {

SequenceExpressionCell::SequenceExpressionCell(Responder * parentResponder) :
  FunctionExpressionCell(),
  Responder(parentResponder),
  m_sequence(nullptr),
  m_numberOfSubCells(1),
  m_selectedSubCell(0),
  m_firstInitialConditionView(EvenOddExpressionCell()),
  m_secondInitialConditionView(EvenOddExpressionCell())
{
}

int SequenceExpressionCell::selectedSubCell() {
  return m_selectedSubCell;
}

void SequenceExpressionCell::selectSubCell(int selectedSubCell) {
  m_selectedSubCell = selectedSubCell;
  m_expressionView.setHighlighted(selectedSubCell == 0);
  m_firstInitialConditionView.setHighlighted(selectedSubCell == 1);
  m_secondInitialConditionView.setHighlighted(selectedSubCell == 2);
  reloadCell();
}

void SequenceExpressionCell::setFunction(Function * f) {
  FunctionExpressionCell::setFunction(f);
  m_sequence = (Sequence *)f;
  m_numberOfSubCells = (int)m_sequence->type()+1;
  if (m_numberOfSubCells > 1) {
    m_firstInitialConditionView.setExpression(m_sequence->firstInitialConditionLayout());
  }
  if (m_numberOfSubCells > 2) {
    m_secondInitialConditionView.setExpression(m_sequence->secondInitialConditionLayout());
  }
}

void SequenceExpressionCell::reloadCell() {
  FunctionExpressionCell::reloadCell();
  m_firstInitialConditionView.setBackgroundColor(backgroundColor());
  m_secondInitialConditionView.setBackgroundColor(backgroundColor());
  if (m_numberOfSubCells > 1 && m_sequence) {
    bool active = m_sequence->isActive();
    KDColor textColor = active ? KDColorBlack : Palette::GreyDark;
    m_firstInitialConditionView.setTextColor(textColor);
    m_secondInitialConditionView.setTextColor(textColor);
  }
}

void SequenceExpressionCell::setHighlighted(bool highlight) {
  m_expressionView.setHighlighted(false);
  m_firstInitialConditionView.setHighlighted(false);
  m_secondInitialConditionView.setHighlighted(false);
  TableViewCell::setHighlighted(highlight);
  if (isHighlighted()) {
    if (m_selectedSubCell == 0) {
      m_expressionView.setHighlighted(true);
    }
    if (m_selectedSubCell == 1) {
      m_firstInitialConditionView.setHighlighted(true);
    }
    if (m_selectedSubCell == 2) {
      m_secondInitialConditionView.setHighlighted(true);
    }
  }
  reloadCell();
}

void SequenceExpressionCell::setEven(bool even) {
  m_expressionView.setEven(even);
  m_firstInitialConditionView.setEven(even);
  m_secondInitialConditionView.setEven(even);
  reloadCell();
}

int SequenceExpressionCell::numberOfSubviews() const {
  return m_numberOfSubCells;
}

View * SequenceExpressionCell::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_expressionView;
  }
  if (index == 1) {
    return &m_firstInitialConditionView;
  }
  return &m_secondInitialConditionView;
}

void SequenceExpressionCell::layoutSubviews() {
  KDCoordinate cellHeight = bounds().height()/m_numberOfSubCells;
  KDRect expressionFrame(k_separatorThickness, 0, bounds().width() - k_separatorThickness, cellHeight);
  m_expressionView.setFrame(expressionFrame);
  expressionFrame = KDRect(k_separatorThickness, cellHeight, bounds().width() - k_separatorThickness, cellHeight);
  m_firstInitialConditionView.setFrame(expressionFrame);
  expressionFrame = KDRect(k_separatorThickness, 2*cellHeight, bounds().width() - k_separatorThickness, cellHeight);
  m_secondInitialConditionView.setFrame(expressionFrame);
}

bool SequenceExpressionCell::handleEvent(Ion::Events::Event event) {
  if (m_selectedSubCell < 2 && event == Ion::Events::Down) {
    selectSubCell(m_selectedSubCell+1);
    return true;
  }
  if (m_selectedSubCell > 0 && event == Ion::Events::Up) {
    selectSubCell(m_selectedSubCell-1);
    return true;
  }
  return false;
}

}
