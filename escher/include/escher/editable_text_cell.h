#ifndef ESCHER_EDITABLE_TEXT_CELL_H
#define ESCHER_EDITABLE_TEXT_CELL_H

#include <escher/responder.h>
#include <escher/highlight_cell.h>
#include <escher/text_field_delegate.h>
#include <escher/text_field.h>

class EditableTextCell : public HighlightCell, public Responder {
public:
  EditableTextCell(Responder * parentResponder = nullptr, TextFieldDelegate * delegate = nullptr, char * draftTextBuffer = nullptr, KDText::FontSize size = KDText::FontSize::Large,
     float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);
  TextField * textField();
  void setMargins(KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    return this;
  }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  constexpr static KDCoordinate k_separatorThickness = 1;
  TextField m_textField;
  char m_textBody[TextField::maxBufferSize()];
  KDCoordinate m_topMargin;
  KDCoordinate m_rightMargin;
  KDCoordinate m_bottomMargin;
  KDCoordinate m_leftMargin;
};

#endif
