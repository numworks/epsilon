#ifndef ESCHER_EVEN_ODD_EDITABLE_TEXT_CELL_H
#define ESCHER_EVEN_ODD_EDITABLE_TEXT_CELL_H

#include <escher/editable_text_cell.h>
#include <escher/even_odd_cell.h>
#include <escher/responder.h>

namespace Escher {

class AbstractEvenOddEditableTextCell : public EvenOddCell, public Responder {
 public:
  AbstractEvenOddEditableTextCell(Responder* parentResponder)
      : EvenOddCell(), Responder(parentResponder) {}
  virtual AbstractEditableTextCell* editableTextCell() = 0;
  Responder* responder() override { return this; }
  const char* text() const override { return editableTextCell()->text(); }

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  constexpr static KDCoordinate k_rightMargin = Escher::Metric::SmallCellMargin;
  const AbstractEditableTextCell* editableTextCell() const {
    return const_cast<AbstractEvenOddEditableTextCell*>(this)
        ->editableTextCell();
  }
  void updateSubviewsBackgroundAfterChangingState() override;
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
};

template <int NumberOfSignificantDigits>
class EvenOddEditableTextCell : public AbstractEvenOddEditableTextCell {
 public:
  EvenOddEditableTextCell(Responder* parentResponder = nullptr,
                          TextFieldDelegate* delegate = nullptr,
                          KDGlyph::Format format = k_smallCellDefaultFormat)
      : AbstractEvenOddEditableTextCell(parentResponder),
        m_editableCell(this, delegate, format) {}

  AbstractEditableTextCell* editableTextCell() override {
    return &m_editableCell;
  }

 private:
  EditableTextCell<NumberOfSignificantDigits> m_editableCell;
};

}  // namespace Escher

#endif
