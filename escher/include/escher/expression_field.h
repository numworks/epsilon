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
  bool editionIsInTextField() const;
  bool isEmpty() const;
  bool inputViewHeightDidChange();
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false);
  void setLayoutInsertionCursorEvent(Ion::Events::Event event) { m_layoutField.setInsertionCursorEvent(event); }

  /* View */
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

private:
  static constexpr int k_textFieldBufferSize = TextField::maxBufferSize();
  static constexpr KDCoordinate k_minimalHeight = 37;
  static constexpr KDCoordinate k_maximalHeight = 0.6*Ion::Display::Height;
  static constexpr KDCoordinate k_horizontalMargin = 5;
  static constexpr KDCoordinate k_verticalMargin = 5;
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
  KDCoordinate inputViewHeight() const;
  KDCoordinate m_inputViewMemoizedHeight;
  TextField m_textField;
  LayoutField m_layoutField;
};

#endif
