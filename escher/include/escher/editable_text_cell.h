#ifndef ESCHER_EDITABLE_TEXT_CELL_H
#define ESCHER_EDITABLE_TEXT_CELL_H

#include <escher/highlight_cell.h>
#include <escher/metric.h>
#include <escher/responder.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <poincare/print_float.h>

namespace Escher {

class EditableTextCell : public HighlightCell, public Responder {
 public:
  EditableTextCell(
      Responder* parentResponder = nullptr,
      InputEventHandlerDelegate* inputEventHandlerDelegate = nullptr,
      TextFieldDelegate* delegate = nullptr,
      KDFont::Size font = KDFont::Size::Large,
      float horizontalAlignment = KDGlyph::k_alignLeft,
      float verticalAlignment = KDGlyph::k_alignCenter,
      KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  TextField* textField();
  void setHighlighted(bool highlight) override;
  Responder* responder() override { return this; }
  const char* text() const override {
    if (!m_textField.isEditing()) {
      return m_textField.text();
    }
    return nullptr;
  }
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  void didBecomeFirstResponder() override;
  KDSize minimalSizeForOptimalDisplay() const override;

 private:
  constexpr static KDCoordinate k_separatorThickness =
      Metric::CellSeparatorThickness;
  TextField m_textField;
  char m_textBody[Poincare::PrintFloat::k_maxFloatCharSize];
};

}  // namespace Escher

#endif
