#ifndef PROBABILITY_CALCULATION_CELL_H
#define PROBABILITY_CALCULATION_CELL_H

#include <escher.h>

namespace Probability {

class CalculationCell : public HighlightCell {
public:
  CalculationCell(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * textFieldDelegate = nullptr);
  Responder * responder() override;
  void setResponder(bool shouldbeResponder);
  void setHighlighted(bool highlight) override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  EditableTextCell * editableTextCell();
  MessageTextView * messageTextView();
  const char * text() const override {
    return m_calculation.text();
  }
private:
  constexpr static KDCoordinate k_margin = 5;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  KDCoordinate calculationCellWidth() const;
  MessageTextView m_text;
  EditableTextCell m_calculation;
  bool m_isResponder;
};

}

#endif
