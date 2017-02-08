#include "sequence_expression_cell.h"

using namespace Shared;

namespace Sequence {

SequenceExpressionCell::SequenceExpressionCell(Responder * parentResponder) :
  Responder(parentResponder),
  EvenOddCell(),
  m_sequence(nullptr),
  m_numberOfSubCells(1),
  m_selectedSubCell(0),
  m_expressionView(EvenOddExpressionCell()),
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

void SequenceExpressionCell::setSequence(Sequence * sequence) {
  m_sequence = sequence;
  m_numberOfSubCells = (int)m_sequence->type()+1;
  bool active = m_sequence->isActive();
  KDColor textColor = active ? KDColorBlack : Palette::GreyDark;
  m_expressionView.setExpression(m_sequence->layout());
  m_expressionView.setTextColor(textColor);
  if (m_numberOfSubCells > 1) {
    m_firstInitialConditionView.setExpression(m_sequence->firstInitialConditionLayout());
    m_firstInitialConditionView.setTextColor(textColor);
  }
  if (m_numberOfSubCells > 2) {
    m_secondInitialConditionView.setExpression(m_sequence->secondInitialConditionLayout());
    m_secondInitialConditionView.setTextColor(textColor);
  }
  layoutSubviews();
}

void SequenceExpressionCell::setHighlighted(bool highlight) {
  TableViewCell::setHighlighted(highlight);
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
  EvenOddCell::setEven(even);
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
  KDCoordinate cellHeight = (bounds().height()-(m_numberOfSubCells-1)*k_separatorThickness)/m_numberOfSubCells;
  KDRect expressionFrame(k_separatorThickness, 0, bounds().width() - k_separatorThickness, cellHeight);
  m_expressionView.setFrame(expressionFrame);
  expressionFrame = KDRect(k_separatorThickness, cellHeight+k_separatorThickness, bounds().width() - k_separatorThickness, cellHeight);
  m_firstInitialConditionView.setFrame(expressionFrame);
  expressionFrame = KDRect(k_separatorThickness, 2*cellHeight+2*k_separatorThickness, bounds().width() - k_separatorThickness, cellHeight);
  m_secondInitialConditionView.setFrame(expressionFrame);
}

void SequenceExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor separatorColor = m_even ? Palette::WallScreen : KDColorWhite;
  // Color the separators
  ctx->fillRect(KDRect(0, 0, k_separatorThickness, bounds().height()), Palette::GreyBright);
  KDCoordinate cellHeight = (bounds().height()-(m_numberOfSubCells-1)*k_separatorThickness)/m_numberOfSubCells;
  if (m_numberOfSubCells > 1) {
    ctx->fillRect(KDRect(k_separatorThickness, cellHeight, bounds().width() - k_separatorThickness, k_separatorThickness), separatorColor);
  }
  if (m_numberOfSubCells > 2) {
    ctx->fillRect(KDRect(k_separatorThickness, 2*cellHeight+k_separatorThickness, bounds().width() - k_separatorThickness, k_separatorThickness), separatorColor);
  }
}

bool SequenceExpressionCell::handleEvent(Ion::Events::Event event) {
  if (m_selectedSubCell < m_numberOfSubCells - 1 && event == Ion::Events::Down) {
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
