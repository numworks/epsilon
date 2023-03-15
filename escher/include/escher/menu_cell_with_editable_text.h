#ifndef ESCHER_MENU_CELL_WITH_EDITABLE_TEXT_H
#define ESCHER_MENU_CELL_WITH_EDITABLE_TEXT_H

#include <escher/chained_text_field_delegate.h>
#include <escher/container.h>
#include <escher/menu_cell.h>
#include <escher/responder.h>
#include <escher/text_field.h>
#include <poincare/print_float.h>

namespace Escher {

class EditableTextWidget : public CellWidget {
 public:
  void setTextField(TextField* textField) { m_textField = textField; }

  // CellWidget
  const View* view() const override { return m_textField; }
  void setBackgroundColor(KDColor color) override {
    m_textField->setBackgroundColor(color);
  }
  bool giveAllWidthAsAccessory() const override { return true; }
  bool alwaysAlignWithLabelAsAccessory() const override { return true; }
  bool preventRightAlignedSubLabel(Type type) const override {
    return type == Type::Accessory;
  }

 private:
  TextField* m_textField;
};

class AbstractWithEditableText : public Responder,
                                 public ChainedTextFieldDelegate {
 public:
  AbstractWithEditableText(
      Responder* parentResponder = nullptr,
      InputEventHandlerDelegate* inputEventHandlerDelegate = nullptr,
      TextFieldDelegate* textFieldDelegate = nullptr);

  void setDelegates(InputEventHandlerDelegate* inputEventHandlerDelegate,
                    TextFieldDelegate* textFieldDelegate);

  void didBecomeFirstResponder() override {
    Container::activeApp()->setFirstResponder(&m_textField);
  }
  TextField* textField() { return &m_textField; }
  virtual void relayout() = 0;

  // ChainedTextFieldDelegate
  void textFieldDidStartEditing(AbstractTextField* textField) override;
  bool textFieldDidFinishEditing(AbstractTextField* textField, const char* text,
                                 Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(AbstractTextField* textField) override;

 protected:
  TextField m_textField;
  char m_textBody[Poincare::PrintFloat::k_maxFloatCharSize];
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
