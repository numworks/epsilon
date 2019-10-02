#ifndef ESCHER_EDITABLE_TEXT_CELL_H
#define ESCHER_EDITABLE_TEXT_CELL_H

#include <escher/responder.h>
#include <escher/highlight_cell.h>
#include <escher/metric.h>
#include <escher/text_field_delegate.h>
#include <escher/text_field.h>
#include <poincare/print_float.h>

class EditableTextCell : public HighlightCell, public Responder {
public:
  EditableTextCell(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * delegate = nullptr, const KDFont * font = KDFont::LargeFont,
     float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);
  TextField * textField();
  void setMargins(KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    return this;
  }
  const char * text() const override {
    if (!m_textField.isEditing()) {
      return m_textField.text();
    }
    return nullptr;
  }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  void didBecomeFirstResponder() override;
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
  TextField m_textField;
  char m_textBody[Poincare::PrintFloat::k_maxFloatCharSize];
  KDCoordinate m_topMargin;
  KDCoordinate m_rightMargin;
  KDCoordinate m_bottomMargin;
  KDCoordinate m_leftMargin;
};

#endif
