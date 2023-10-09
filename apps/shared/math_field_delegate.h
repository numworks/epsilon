#ifndef SHARED_MATH_INPUT_FIELD_DELEGATE_H
#define SHARED_MATH_INPUT_FIELD_DELEGATE_H

#include <escher/abstract_text_field.h>
#include <escher/app.h>
#include <escher/layout_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/text_field_delegate.h>

namespace Shared {

class AbstractMathFieldDelegate {
 protected:
  virtual bool isAcceptableExpression(const Poincare::Expression expression);
  bool isAcceptableText(const char* text);
  static bool ExpressionCanBeSerialized(const Poincare::Expression expression,
                                        bool replaceAns,
                                        Poincare::Expression ansExpression,
                                        Poincare::Context* context);
  virtual CodePoint defaultXNT();
  bool handleEventForField(Escher::EditableField* field,
                           Ion::Events::Event event);
};

class MathLayoutFieldDelegate : public AbstractMathFieldDelegate,
                                public Escher::LayoutFieldDelegate {
 public:
  static MathLayoutFieldDelegate* Default();

  // LayoutFieldDelegate
  bool layoutFieldDidReceiveEvent(Escher::LayoutField* layoutField,
                                  Ion::Events::Event event) override;
  bool layoutFieldDidFinishEditing(Escher::LayoutField* layoutField,
                                   Ion::Events::Event event) override;

  bool layoutFieldHasSyntaxError(Escher::LayoutField* layoutField);
};

class MathTextFieldDelegate : public AbstractMathFieldDelegate,
                              public Escher::TextFieldDelegate {
 public:
  static MathTextFieldDelegate* Default();

  // TextFieldDelegate
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  template <typename T>
  static T ParseInputFloatValue(const char* text);
  template <typename T>
  static bool HasUndefinedValue(T value, bool enablePlusInfinity = false,
                                bool enableMinusInfinity = false);
};

}  // namespace Shared

#endif
