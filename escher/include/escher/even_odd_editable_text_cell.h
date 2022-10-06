#ifndef ESCHER_EVEN_ODD_EDITABLE_TEXT_CELL_H
#define ESCHER_EVEN_ODD_EDITABLE_TEXT_CELL_H

#include <escher/editable_text_cell.h>
#include <escher/even_odd_cell.h>
#include <escher/responder.h>

namespace Escher {

class EvenOddEditableTextCell : public EvenOddCell, public Responder {
public:
  EvenOddEditableTextCell(Responder * parentResponder = nullptr,
                          InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr,
                          TextFieldDelegate * delegate = nullptr,
                          KDFont::Size font = KDFont::Size::Large,
                          float horizontalAlignment = KDContext::k_alignRight,
                          float verticalAlignment = KDContext::k_alignCenter,
                          KDCoordinate topMargin = 0,
                          KDCoordinate rightMargin = 0,
                          KDCoordinate bottomMargin = 0,
                          KDCoordinate leftMargin = 0);
  EditableTextCell * editableTextCell();
  Responder * responder() override { return this; }
  const char * text() const override { return m_editableCell.text(); }
  void didBecomeFirstResponder() override;
  void setFont(KDFont::Size font) { m_editableCell.textField()->setFont(font); }

private:
  void updateSubviewsBackgroundAfterChangingState() override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  EditableTextCell m_editableCell;
};

}  // namespace Escher

#endif
