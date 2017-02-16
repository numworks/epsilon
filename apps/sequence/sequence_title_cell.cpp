#include "sequence_title_cell.h"

using namespace Shared;

namespace Sequence {

SequenceTitleCell::SequenceTitleCell(Responder * parentResponder) :
  FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator),
  Responder(parentResponder),
  m_numberOfSubCells(1),
  m_selectedSubCell(0),
  m_firstInitialConditionView(KDText::FontSize::Large, 0.5f, 0.5f),
  m_secondInitialConditionView(KDText::FontSize::Large, 0.5f, 0.5f)
{
}

int SequenceTitleCell::selectedSubCell() {
  return m_selectedSubCell;
}

void SequenceTitleCell::selectSubCell(int selectedSubCell) {
  m_selectedSubCell = selectedSubCell;
  m_bufferTextView.setHighlighted(selectedSubCell == 0);
  m_firstInitialConditionView.setHighlighted(selectedSubCell == 1);
  m_secondInitialConditionView.setHighlighted(selectedSubCell == 2);
  reloadCell();
}

void SequenceTitleCell::setHighlighted(bool highlight) {
  m_bufferTextView.setHighlighted(false);
  m_firstInitialConditionView.setHighlighted(false);
  m_secondInitialConditionView.setHighlighted(false);
  TableViewCell::setHighlighted(highlight);
  if (isHighlighted()) {
    if (m_selectedSubCell == 0) {
      m_bufferTextView.setHighlighted(true);
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

void SequenceTitleCell::setEven(bool even) {
  m_bufferTextView.setEven(even);
  m_firstInitialConditionView.setEven(even);
  m_secondInitialConditionView.setEven(even);
  reloadCell();
}

int SequenceTitleCell::numberOfSubviews() const {
  return m_numberOfSubCells;
}

View * SequenceTitleCell::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_bufferTextView;
  }
  if (index == 1) {
    return &m_firstInitialConditionView;
  }
  return &m_secondInitialConditionView;
}

void SequenceTitleCell::layoutSubviews() {
  KDCoordinate cellHeight = bounds().height()/m_numberOfSubCells;
  KDRect expressionFrame(k_colorIndicatorThickness, 0, bounds().width() - k_separatorThickness, cellHeight);
  m_bufferTextView.setFrame(expressionFrame);
  expressionFrame = KDRect(k_colorIndicatorThickness, cellHeight, bounds().width() - k_separatorThickness, cellHeight);
  m_firstInitialConditionView.setFrame(expressionFrame);
  expressionFrame = KDRect(k_colorIndicatorThickness, 2*cellHeight, bounds().width() - k_separatorThickness, cellHeight);
  m_secondInitialConditionView.setFrame(expressionFrame);
}

bool SequenceTitleCell::handleEvent(Ion::Events::Event event) {
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
