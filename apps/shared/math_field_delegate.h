#ifndef SHARED_MATH_INPUT_FIELD_DELEGATE_H
#define SHARED_MATH_INPUT_FIELD_DELEGATE_H

#include <escher/abstract_text_field.h>
#include <escher/app.h>
#include <escher/layout_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/text_field_delegate.h>

namespace Shared {

class AbstractMathFieldDelegate {
 public:
  virtual CodePoint XNT();

  bool fieldDidReceiveEvent(Escher::EditableField* field,
                            Escher::Responder* responder,
                            Ion::Events::Event event);

 protected:
  virtual bool isAcceptableExpression(const Poincare::Expression expression);
  bool isAcceptableText(const char* text);
  static bool ExpressionCanBeSerialized(const Poincare::Expression expression,
                                        bool replaceAns,
                                        Poincare::Expression ansExpression,
                                        Poincare::Context* context);
};

class MathLayoutFieldDelegate : public AbstractMathFieldDelegate,
                                public Escher::LayoutFieldDelegate {
 public:
  static MathLayoutFieldDelegate* Default();

  // LayoutFieldDelegate
  bool layoutFieldShouldFinishEditing(Escher::LayoutField* layoutField,
                                      Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(Escher::LayoutField* layoutField,
                                  Ion::Events::Event event) override;
};

class MathTextFieldDelegate : public AbstractMathFieldDelegate,
                              public Escher::TextFieldDelegate {
 public:
  static MathTextFieldDelegate* Default();

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;

  template <typename T>
  static T ParseInputtedFloatValue(const char* text);
  template <typename T>
  static bool HasUndefinedValue(T value, bool enablePlusInfinity = false,
                                bool enableMinusInfinity = false);
};

}  // namespace Shared

#endif
