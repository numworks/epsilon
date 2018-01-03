#ifndef PROBABILITY_CALCULATION_CELL_H
#define PROBABILITY_CALCULATION_CELL_H

#include <escher.h>

namespace Probability {

class CalculationCell : public HighlightCell {
public:
  CalculationCell(Responder * parentResponder = nullptr, char * draftTextBuffer = nullptr, TextFieldDelegate * textFieldDelegate = nullptr);
  Responder * responder() override;
  void setResponder(bool shouldbeResponder);
  void setHighlighted(bool highlight) override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  EditableTextCell * editableTextCell();
  MessageTextView * messageTextView();
private:
  constexpr static KDCoordinate k_margin = 5;
  constexpr static KDCoordinate k_minTextFieldWidth = 4*KDText::charSize().width()+TextCursorView::k_width;
  constexpr static KDCoordinate k_maxTextFieldWidth = 14*KDText::charSize().width()+TextCursorView::k_width;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  KDCoordinate calculationCellWidth() const;
  MessageTextView m_text;
  EditableTextCell m_calculation;
  bool m_isResponder;
};

}

#endif
