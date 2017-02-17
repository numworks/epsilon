#include "sequence_title_cell.h"

using namespace Shared;

namespace Sequence {

SequenceTitleCell::SequenceTitleCell(Responder * parentResponder) :
  FunctionTitleCell(FunctionTitleCell::Orientation::VerticalIndicator),
  Responder(parentResponder),
  m_numberOfSubCells(1),
  m_selectedSubCell(0),
  m_definitionView(KDText::FontSize::Large, 0.5f, 0.5f),
  m_firstInitialConditionView(KDText::FontSize::Large, 0.5f, 0.5f),
  m_secondInitialConditionView(KDText::FontSize::Large, 0.5f, 0.5f)
{
}

void SequenceTitleCell::setDefinitionText(const char * title) {
  m_definitionView.setText(title);
}

void SequenceTitleCell::setFirstInitialConditionText(const char * textContent) {
  m_firstInitialConditionView.setText(textContent);
}

void SequenceTitleCell::setSecondInitialConditionText(const char * textContent) {
  m_secondInitialConditionView.setText(textContent);
}

void SequenceTitleCell::setColor(KDColor color) {
  FunctionTitleCell::setColor(color);
  m_definitionView.setTextColor(color);
  m_firstInitialConditionView.setTextColor(color);
  m_secondInitialConditionView.setTextColor(color);
}

void SequenceTitleCell::setNumberOfSubCells(int numberOfSubcells) {
  m_numberOfSubCells = numberOfSubcells;
  layoutSubviews();
}

int SequenceTitleCell::selectedSubCell() {
  return m_selectedSubCell;
}

void SequenceTitleCell::selectSubCell(int selectedSubCell) {
  m_selectedSubCell = selectedSubCell;
  m_definitionView.setHighlighted(selectedSubCell == 0);
  m_firstInitialConditionView.setHighlighted(selectedSubCell == 1);
  m_secondInitialConditionView.setHighlighted(selectedSubCell == 2);
  reloadCell();
}

void SequenceTitleCell::setHighlighted(bool highlight) {
  TableViewCell::setHighlighted(highlight);
  m_definitionView.setHighlighted(false);
  m_firstInitialConditionView.setHighlighted(false);
  m_secondInitialConditionView.setHighlighted(false);
  TableViewCell::setHighlighted(highlight);
  if (isHighlighted()) {
    if (m_selectedSubCell == 0) {
      m_definitionView.setHighlighted(true);
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
  EvenOddCell::setEven(even);
  m_definitionView.setEven(even);
  m_firstInitialConditionView.setEven(even);
  m_secondInitialConditionView.setEven(even);
  reloadCell();
}

int SequenceTitleCell::numberOfSubviews() const {
  return m_numberOfSubCells;
}

View * SequenceTitleCell::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_definitionView;
  }
  if (index == 1) {
    return &m_firstInitialConditionView;
  }
  return &m_secondInitialConditionView;
}

void SequenceTitleCell::layoutSubviews() {
  KDCoordinate cellHeight = (bounds().height()-(m_numberOfSubCells-1)*k_separatorThickness)/m_numberOfSubCells;
  KDRect expressionFrame(k_colorIndicatorThickness, 0, bounds().width() - k_separatorThickness, cellHeight);
  m_definitionView.setFrame(expressionFrame);
  expressionFrame = KDRect(k_colorIndicatorThickness, cellHeight+k_separatorThickness, bounds().width() - k_separatorThickness, cellHeight);
  m_firstInitialConditionView.setFrame(expressionFrame);
  expressionFrame = KDRect(k_colorIndicatorThickness, 2*cellHeight+2*k_separatorThickness, bounds().width() - k_separatorThickness, cellHeight);
  m_secondInitialConditionView.setFrame(expressionFrame);
}

void SequenceTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor separatorColor = m_even ? Palette::WallScreen : KDColorWhite;
  KDCoordinate cellHeight = (bounds().height()-(m_numberOfSubCells-1)*k_separatorThickness)/m_numberOfSubCells;
  if (m_numberOfSubCells > 1) {
    ctx->fillRect(KDRect(k_separatorThickness, cellHeight, bounds().width() - k_separatorThickness, k_separatorThickness), separatorColor);
  }
  if (m_numberOfSubCells > 2) {
    ctx->fillRect(KDRect(k_separatorThickness, 2*cellHeight+k_separatorThickness, bounds().width() - k_separatorThickness, k_separatorThickness), separatorColor);
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
  return false;
}

}
