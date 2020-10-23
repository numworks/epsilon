#ifndef PROBABILITY_CALCULATION_CELL_H
#define PROBABILITY_CALCULATION_CELL_H

namespace Probability {

class CalculationCell : public Escher::HighlightCell {
public:
  CalculationCell(Escher::Responder * parentResponder = nullptr, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, Escher::TextFieldDelegate * textFieldDelegate = nullptr);
  Escher::Responder * responder() override;
  void setResponder(bool shouldbeResponder);
  void setHighlighted(bool highlight) override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  Escher::EditableTextCell * editableTextCell();
  Escher::MessageTextView * messageTextView();
  const char * text() const override {
    return m_calculation.text();
  }
private:
  constexpr static KDCoordinate k_margin = 5;
  int numberOfSubviews() const override;
  Escher::View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  KDCoordinate calculationCellWidth() const;
  Escher::MessageTextView m_text;
  Escher::EditableTextCell m_calculation;
  bool m_isResponder;
};

}

#endif
