#ifndef ESCHER_EXPRESSION_FIELD_H
#define ESCHER_EXPRESSION_FIELD_H

#include <escher/layout_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/metric.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <poincare/layout.h>

namespace Escher {

class ExpressionField : public WithBlinkingTextCursor<Responder>, public View {
public:
  ExpressionField(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandler, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate, float horizontalAlignment = KDContext::k_alignLeft, float verticalAlignment = KDContext::k_alignCenter);

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
  void reload();
  bool handleEventWithText(const char * text, bool indentation = false, bool forceCursorRightOfText = false);
 size_t dumpContent(char * buffer, size_t bufferSize, int * cursorOffset, int * position);
 void restoreContent(const char * buffer, size_t size, int * cursorOffset, int * position);
  void setBackgroundColor(KDColor backgroundColor);
  void putCursorLeftOfField();
  void setTextEditionBuffer(char * buffer, size_t bufferSize) { m_textField.setEditionBuffer(buffer, bufferSize); }

  /* View */
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  KDSize minimalSizeForOptimalDisplay() const override;

  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

protected:
  void layoutSubviewsInRect(KDRect rect, bool force);
  TextField m_textField;
  LayoutField m_layoutField;

private:
  constexpr static int k_textFieldBufferSize = TextField::MaxBufferSize();
  virtual KDCoordinate inputViewHeight() const;
  TextCursorView * textCursorView() override {
    return editionIsInTextField() ? m_textField.textCursorView() : m_layoutField.textCursorView();
  }
  KDCoordinate m_inputViewMemoizedHeight;
};

}

#endif
