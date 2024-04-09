#ifndef ESCHER_EDITABLE_TEXT_CELL_H
#define ESCHER_EDITABLE_TEXT_CELL_H

#include <escher/highlight_cell.h>
#include <escher/metric.h>
#include <escher/responder.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <poincare/print_float.h>

namespace Escher {

class AbstractEditableTextCell : public HighlightCell, public Responder {
 public:
  AbstractEditableTextCell(Responder* parentResponder)
      : HighlightCell(), Responder(parentResponder) {}
  virtual TextField* textField() = 0;
  void setHighlighted(bool highlight) override;
  Responder* responder() override { return this; }
  const char* text() const override;
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  void didBecomeFirstResponder() override;
  KDSize minimalSizeForOptimalDisplay() const override;

 private:
  constexpr static KDCoordinate k_separatorThickness =
      Metric::CellSeparatorThickness;
};

template <int NumberOfSignificantDigits =
              Poincare::PrintFloat::k_maxNumberOfSignificantDigits>
class EditableTextCell : public AbstractEditableTextCell {
 public:
  EditableTextCell(Responder* parentResponder = nullptr,
                   TextFieldDelegate* delegate = nullptr,
                   KDGlyph::Format format = {})
      : AbstractEditableTextCell(parentResponder),
        m_textField(this, m_textBody, k_bufferSize, delegate, format) {
    m_textBody[0] = 0;
  }

  constexpr static size_t k_bufferSize =
      Poincare::PrintFloat::charSizeForFloatsWithPrecision(
          NumberOfSignificantDigits);

  TextField* textField() override { return &m_textField; }

 private:
  TextField m_textField;
  char m_textBody[k_bufferSize];
};

}  // namespace Escher

#endif
