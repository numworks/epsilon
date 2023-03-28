#ifndef ESCHER_MENU_CELL_WITH_EDITABLE_TEXT_H
#define ESCHER_MENU_CELL_WITH_EDITABLE_TEXT_H

#include <escher/chained_text_field_delegate.h>
#include <escher/container.h>
#include <escher/menu_cell.h>
#include <escher/responder.h>
#include <escher/text_field.h>
#include <poincare/print_float.h>

namespace Escher {

/* MenuCells containing an editable TextField have a specific derived class
 * called MenuCellWithEditableField, because they need to be a
 * TextFieldDelegate, so that they can be re-layouted when the TextField starts
 * or finishes edition. */

class EditableTextWidget : public CellWidget {
 public:
  void setTextField(TextField* textField) { m_textField = textField; }
  bool isEditing() const { return m_textField->isEditing(); }

  // CellWidget
  const View* view() const override { return m_textField; }
  void setHighlighted(bool highlighted) override {
    m_textField->setBackgroundColor(
        AbstractMenuCell::BackgroundColor(highlighted));
  }
  bool isAnEditableTextField() const override { return true; }
  bool alwaysAlignWithLabelAsAccessory() const override { return true; }
  bool preventRightAlignedSubLabel(Type type) const override {
    return type == Type::Accessory;
  }

  KDCoordinate minimalWidth() const {
    return Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
               Poincare::Preferences::ShortNumberOfSignificantDigits + 1) *
               KDFont::GlyphWidth(m_textField->font()) +
           TextCursorView::k_width;
  }

 private:
  TextField* m_textField;
};

/* This abstract class only purpose is to make the templated class
 * MenuCellWithEditableText thinner. */
class AbstractWithEditableText : public Responder,
                                 public ChainedTextFieldDelegate {
 public:
  AbstractWithEditableText(
      Responder* parentResponder = nullptr,
      InputEventHandlerDelegate* inputEventHandlerDelegate = nullptr,
      TextFieldDelegate* textFieldDelegate = nullptr);

  void setEditable(bool isEditable) { m_editable = isEditable; }
  void setDelegates(InputEventHandlerDelegate* inputEventHandlerDelegate,
                    TextFieldDelegate* textFieldDelegate);

  void didBecomeFirstResponder() override {
    if (m_editable) {
      Container::activeApp()->setFirstResponder(&m_textField);
    }
  }
  TextField* textField() { return &m_textField; }
  virtual void relayout() = 0;

  // ChainedTextFieldDelegate
  void textFieldDidStartEditing(AbstractTextField* textField) override;
  bool textFieldDidFinishEditing(AbstractTextField* textField, const char* text,
                                 Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(AbstractTextField* textField) override;
  bool textFieldIsEditable(AbstractTextField* textField) override {
    return m_editable;
  }

 protected:
  TextField m_textField;
  char m_textBody[Poincare::PrintFloat::k_maxFloatCharSize];
  bool m_editable;
};

template <typename Label, typename SubLabel = EmptyCellWidget>
class MenuCellWithEditableText
    : public MenuCell<Label, SubLabel, EditableTextWidget>,
      public AbstractWithEditableText {
 public:
  MenuCellWithEditableText(
      Responder* parentResponder = nullptr,
      InputEventHandlerDelegate* inputEventHandlerDelegate = nullptr,
      TextFieldDelegate* textFieldDelegate = nullptr)
      : MenuCell<Label, SubLabel, EditableTextWidget>(),
        AbstractWithEditableText(parentResponder, inputEventHandlerDelegate,
                                 textFieldDelegate) {
    this->accessory()->setTextField(&m_textField);
  }

  Responder* responder() override { return this; }
  const char* text() const override {
    return !m_textField.isEditing() ? m_textField.text() : nullptr;
  }
  void relayout() override { this->layoutSubviews(); }
};

}  // namespace Escher

#endif
