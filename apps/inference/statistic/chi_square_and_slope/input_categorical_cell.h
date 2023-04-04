#ifndef INFERENCE_INPUT_CATEGORICAL_CELL_H
#define INFERENCE_INPUT_CATEGORICAL_CELL_H

#include <apps/i18n.h>
#include <escher/menu_cell_with_editable_text.h>

using namespace Escher;

namespace Inference {

// Cell with left and right margins for InputCategoricalController

class InputCategoricalCell : public HighlightCell {
 public:
  InputCategoricalCell(
      Responder* parentResponder = nullptr,
      InputEventHandlerDelegate* inputEventHandlerDelegate = nullptr,
      TextFieldDelegate* textFieldDelegate = nullptr);

  // View
  KDSize minimalSizeForOptimalDisplay() const override;
  void drawRect(KDContext* ctx, KDRect rect) const override;

  // HighlightCell
  void setHighlighted(bool highlight) override;
  Responder* responder() override { return m_innerCell.responder(); }
  const char* text() const override { return m_innerCell.text(); }
  Poincare::Layout layout() const override { return m_innerCell.layout(); }

  TextField* textField() { return m_innerCell.textField(); }
  void setMessages(I18n::Message labelMessage,
                   I18n::Message subLabelMessage = I18n::Message::Default);

 private:
  // View
  int numberOfSubviews() const override { return 1; }
  HighlightCell* subviewAtIndex(int i) override { return &m_innerCell; }
  void layoutSubviews(bool force) override;

  Escher::MenuCellWithEditableText<Escher::MessageTextView,
                                   Escher::MessageTextView>
      m_innerCell;
};

}  // namespace Inference

#endif
