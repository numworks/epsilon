#ifndef INFERENCE_STATISTIC_EXPRESSION_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H
#define INFERENCE_STATISTIC_EXPRESSION_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H

#include <escher/chained_text_field_delegate.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/responder.h>
#include <escher/text_field.h>
#include <poincare/print_float.h>

namespace Inference {

class ExpressionCellWithEditableTextWithMessage : public Escher::ExpressionTableCellWithMessage,
                                                  public Escher::ChainedTextFieldDelegate {
public:
  ExpressionCellWithEditableTextWithMessage(
      Escher::Responder * parent = nullptr,
      Escher::InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr,
      Escher::TextFieldDelegate * textFieldDelegate = nullptr);

  void didBecomeFirstResponder() override;
  Responder * responder() override { return this; }

  const View * accessoryView() const override { return &m_textField; }
  Escher::TextField * textField() { return &m_textField; }

  bool giveAccessoryAllWidth() const override { return true; }
  bool shouldAlignLabelAndAccessory() const override { return true; }
  bool shouldAlignSublabelRight() const override { return false; }
  KDCoordinate accessoryMinimalWidthOverridden() const override {
    // TODO this is redundant with MessageTableCellWithEditableText
    return Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
               Poincare::Preferences::ShortNumberOfSignificantDigits + 1) *
               KDFont::GlyphWidth(m_textField.font()) +
           Escher::TextCursorView::k_width;
  }
  void setAccessoryText(const char * text);
  void setHighlighted(bool highlight) override;
  bool shouldHideSublabel() override { return singleRowMode() && m_textField.isEditing(); }

  void textFieldDidStartEditing(Escher::AbstractTextField * textField) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(Escher::AbstractTextField * textField) override;

  void setDelegates(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                    Escher::TextFieldDelegate * textFieldDelegate);

private:
  char m_textBody[Poincare::PrintFloat::k_maxFloatCharSize];
  Escher::TextField m_textField;
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_EXPRESSION_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H */
