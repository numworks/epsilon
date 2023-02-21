#ifndef DISTRIBUTIONS_PROBABILITY_CALCULATION_CELL_H
#define DISTRIBUTIONS_PROBABILITY_CALCULATION_CELL_H

#include <escher/editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/message_text_view.h>
#include <escher/text_field_delegate.h>

namespace Distributions {

class CalculationCell : public Escher::HighlightCell {
 public:
  CalculationCell(
      Escher::Responder* parentResponder = nullptr,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate = nullptr,
      Escher::TextFieldDelegate* textFieldDelegate = nullptr);
  Escher::Responder* responder() override;
  void setResponder(bool shouldbeResponder);
  void setHighlighted(bool highlight) override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext* ctx, KDRect rect) const override;
  Escher::EditableTextCell* editableTextCell();
  Escher::MessageTextView* messageTextView();
  const char* text() const override { return m_calculation.text(); }

 private:
  constexpr static KDCoordinate k_margin = 5;
  constexpr static KDCoordinate k_textBottomOffset = 1;
  int numberOfSubviews() const override;
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  KDCoordinate calculationCellWidth() const;
  Escher::MessageTextView m_text;
  Escher::EditableTextCell m_calculation;
  bool m_isResponder;
};

}  // namespace Distributions

#endif
