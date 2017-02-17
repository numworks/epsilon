#include "sequence_cell.h"

namespace Sequence {

SequenceCell::SequenceCell(Responder * parentResponder) :
  EvenOddCell(),
  Responder(parentResponder),
  m_numberOfSubCells(1),
  m_selectedSubCell(0),
  m_sequence(nullptr)
{
}

int SequenceCell::numberOfSubCells() {
  return m_numberOfSubCells;
}

int SequenceCell::selectedSubCell() {
  return m_selectedSubCell;
}

void SequenceCell::selectSubCell(int selectedSubCell) {
  m_selectedSubCell = selectedSubCell;
  viewAtIndex(0)->setHighlighted(selectedSubCell == 0);
  viewAtIndex(1)->setHighlighted(selectedSubCell == 1);
  viewAtIndex(2)->setHighlighted(selectedSubCell == 2);
  reloadCell();
}

void SequenceCell::setSequence(Sequence * sequence) {
  m_sequence = sequence;
  m_numberOfSubCells = (int)m_sequence->type()+1;
  layoutSubviews();
}

void SequenceCell::setHighlighted(bool highlight) {
  TableViewCell::setHighlighted(highlight);
  viewAtIndex(0)->setHighlighted(false);
  viewAtIndex(1)->setHighlighted(false);
  viewAtIndex(2)->setHighlighted(false);
  TableViewCell::setHighlighted(highlight);
  if (isHighlighted()) {
    viewAtIndex(m_selectedSubCell)->setHighlighted(true);
  }
  reloadCell();
}

void SequenceCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  viewAtIndex(0)->setEven(even);
  viewAtIndex(1)->setEven(even);
  viewAtIndex(2)->setEven(even);
  reloadCell();
}

int SequenceCell::numberOfSubviews() const {
  return m_numberOfSubCells;
}

View * SequenceCell::subviewAtIndex(int index) {
  return viewAtIndex(index);
}

void SequenceCell::layoutSubviews() {
  KDCoordinate cellHeight = (bounds().height()-(m_numberOfSubCells-1)*k_separatorThickness)/m_numberOfSubCells;
  KDRect expressionFrame(k_separatorThickness, 0, bounds().width() - k_separatorThickness, cellHeight);
  viewAtIndex(0)->setFrame(expressionFrame);
  expressionFrame = KDRect(k_separatorThickness, cellHeight+k_separatorThickness, bounds().width() - k_separatorThickness, cellHeight);
  viewAtIndex(1)->setFrame(expressionFrame);
  expressionFrame = KDRect(k_separatorThickness, 2*cellHeight+2*k_separatorThickness, bounds().width() - k_separatorThickness, cellHeight);
  viewAtIndex(2)->setFrame(expressionFrame);
}

}
