#ifndef ESCHER_EVEN_ODD_EDITABLE_TEXT_CELL_H
#define ESCHER_EVEN_ODD_EDITABLE_TEXT_CELL_H

#include <escher/responder.h>
#include <escher/even_odd_cell.h>
#include <escher/editable_text_cell.h>

class EvenOddEditableTextCell : public EvenOddCell, public Responder {
public:
  EvenOddEditableTextCell(Responder * parentResponder = nullptr, TextFieldDelegate * delegate = nullptr, char * draftTextBuffer = nullptr, KDText::FontSize size = KDText::FontSize::Large, float horizontalAlignment = 1.0f, float verticalAlignment = 0.5f, KDCoordinate topMargin = 0, KDCoordinate rightMargin = 0, KDCoordinate bottomMargin = 0, KDCoordinate leftMargin = 0);
  EditableTextCell * editableTextCell();
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  virtual Responder * responder() override {
    return this;
  }
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
private:
  EditableTextCell m_editableCell;
};

#endif
