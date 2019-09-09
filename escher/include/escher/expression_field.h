#ifndef ESCHER_EXPRESSION_FIELD_H
#define ESCHER_EXPRESSION_FIELD_H

#include <escher/layout_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/metric.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <poincare/layout.h>

class ExpressionField : public Responder, public View {
public:
  ExpressionField(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandler, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate);

  void setEditing(bool isEditing, bool reinitDraftBuffer = true);
  bool isEditing() const;
  /* Warning: this function is VERY dangerous! Indeed: sometimes the
   * m_layoutField might overflow the m_textBuffer once serialized
   * and still have been accepted before because the model can hold a longer
   * buffer. This is the case in the application 'Calculation' and we do not
   * use text() there... TODO: change text() for fillTextInBuffer?*/
  const char * text();
  void setText(const char * text);
  void reload();
  bool editionIsInTextField() const;
  bool isEmpty() const;
  bool heightIsMaximal() const;
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false);

  /* View */
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;

private:
  static constexpr int k_textFieldBufferSize = TextField::maxBufferSize();
  static constexpr KDCoordinate k_textFieldHeight = 37;
  static constexpr KDCoordinate k_horizontalMargin = 5;
  static constexpr KDCoordinate k_verticalMargin = 5;
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
  KDCoordinate inputViewHeight() const;
  KDCoordinate maximalHeight() const;
  TextField m_textField;
  LayoutField m_layoutField;
};

#endif
