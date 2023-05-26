#ifndef SHARED_MATH_INPUT_FIELD_DELEGATE_H
#define SHARED_MATH_INPUT_FIELD_DELEGATE_H

#include <escher/abstract_text_field.h>
#include <escher/app.h>
#include <escher/container.h>
#include <escher/layout_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/text_field_delegate.h>

namespace Shared {

class MathFieldDelegate : public Escher::TextFieldDelegate,
                          public Escher::LayoutFieldDelegate {
 public:
  static MathFieldDelegate* Default();

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;

  // LayoutFieldDelegate
  bool layoutFieldShouldFinishEditing(Escher::LayoutField* layoutField,
                                      Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(Escher::LayoutField* layoutField,
                                  Ion::Events::Event event) override;

  bool fieldDidReceiveEvent(Escher::EditableField* field,
                            Escher::Responder* responder,
                            Ion::Events::Event event);
  virtual CodePoint XNT();

  template <typename T>
  static T ParseInputtedFloatValue(const char* text);
  template <typename T>
  static bool HasUndefinedValue(T value, bool enablePlusInfinity = false,
                                bool enableMinusInfinity = false);

 protected:
  virtual bool isAcceptableExpression(const Poincare::Expression expression);
  bool isAcceptableText(const char* text);

  static bool ExpressionCanBeSerialized(const Poincare::Expression expression,
                                        bool replaceAns,
                                        Poincare::Expression ansExpression,
                                        Poincare::Context* context);

 private:
  static Escher::App* App() { return Escher::Container::activeApp(); }
};

}  // namespace Shared

#endif
