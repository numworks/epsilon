#ifndef SHARED_MATH_INPUT_FIELD_DELEGATE_H
#define SHARED_MATH_INPUT_FIELD_DELEGATE_H

#include <escher/abstract_text_field.h>
#include <escher/app.h>
#include <escher/layout_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/text_field_delegate.h>
#include <poincare/expression.h>

namespace Shared {

class AbstractMathFieldDelegate {
 public:
  AbstractMathFieldDelegate() : m_currentXNTIndex(-1) {}

 protected:
  virtual bool isAcceptableExpression(const Poincare::UserExpression expression,
                                      Poincare::Context* context);
  bool isAcceptableText(const char* text, Poincare::Context* context);
  virtual CodePoint defaultXNT();
  bool handleEventForField(Escher::EditableField* field,
                           Ion::Events::Event event);
  void updateXNTIndex(Ion::Events::Event event);

 private:
  int m_currentXNTIndex;
};

class MathLayoutFieldDelegate : public AbstractMathFieldDelegate,
                                public Escher::LayoutFieldDelegate {
 public:
  using AbstractMathFieldDelegate::AbstractMathFieldDelegate;
  static MathLayoutFieldDelegate* Default();

  // LayoutFieldDelegate
  bool layoutFieldDidReceiveEvent(Escher::LayoutField* layoutField,
                                  Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(Escher::LayoutField* layoutField,
                                   Ion::Events::Event event) override;
  void updateRepetitionIndexes(Escher::LayoutField* layoutField,
                               Ion::Events::Event event) override {
    updateXNTIndex(event);
  }

  bool isAcceptableLayout(Poincare::Layout layout, Poincare::Context* context);
};

class MathTextFieldDelegate : public AbstractMathFieldDelegate,
                              public Escher::TextFieldDelegate {
 public:
  using AbstractMathFieldDelegate::AbstractMathFieldDelegate;
  static MathTextFieldDelegate* Default();

  // TextFieldDelegate
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;
  void updateRepetitionIndexes(Escher::AbstractTextField* textField,
                               Ion::Events::Event event) override {
    updateXNTIndex(event);
  }

  template <typename T>
  static T ParseInputFloatValue(const char* text);
  template <typename T>
  static bool HasUndefinedValue(T value, bool enablePlusInfinity = false,
                                bool enableMinusInfinity = false);
};

}  // namespace Shared

#endif
