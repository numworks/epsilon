#pragma once

#include <escher/abstract_text_field.h>
#include <escher/app.h>
#include <escher/layout_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/text_field_delegate.h>
#include <poincare/syntax_error.h>
#include <poincare/user_expression.h>

namespace Shared {

class AbstractMathFieldDelegate {
 public:
  AbstractMathFieldDelegate() : m_currentXNTIndex(-1) {}

 protected:
  virtual bool isAcceptableExpression(
      const Poincare::UserExpression& expression,
      const Poincare::SymbolContext& symbolContext);
  bool isAcceptableTextOrWarning(const char* text,
                                 const Poincare::SymbolContext& symbolContext);
  virtual CodePoint defaultXNT();
  bool handleEventForField(Escher::EditableField* field,
                           Ion::Events::Event event);
  void updateXNTIndex(Ion::Events::Event event);
  void displayWarningForSyntaxError(
      Poincare::SyntaxError error = Poincare::SyntaxError::Default) const;

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
  /* Return true if the layout is acceptable, return false and display a warning
   * otherwise. */
  bool isAcceptableLayoutOrWarning(
      const Poincare::Layout& layout,
      const Poincare::SymbolContext& symbolContext);
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
