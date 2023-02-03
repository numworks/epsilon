#ifndef ESCHER_EXPRESSION_FIELD_H
#define ESCHER_EXPRESSION_FIELD_H

#include <escher/layout_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/metric.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <poincare/layout.h>

namespace Escher {

class ExpressionField : public LayoutField {
public:
  ExpressionField(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandler, LayoutFieldDelegate * layoutFieldDelegate, float horizontalAlignment = KDContext::k_alignLeft, float verticalAlignment = KDContext::k_alignCenter);

  void setEditing(bool isEditing, bool reinitDraftBuffer = true);
  /* Warning: this function is VERY dangerous! Indeed: sometimes the
   * m_layoutField might overflow the m_textBuffer once serialized
   * and still have been accepted before because the model can hold a longer
   * buffer. This is the case in the application 'Calculation' and we do not
   * use text() there... TODO: change text() for fillTextInBuffer?*/
  const char * text();
  void setText(const char * text);
  bool inputViewHeightDidChange();
  void reload();
  void restoreContent(const char * buffer, size_t size, int * cursorOffset, int * position);
  void setTextEditionBuffer(char * buffer, size_t bufferSize) { m_textField.setEditionBuffer(buffer, bufferSize); }

  /* View */
  KDSize minimalSizeForOptimalDisplay() const override;

  /* Responder */
  void didBecomeFirstResponder() override;

protected:
  TextField m_textField;

private:
  constexpr static int k_textFieldBufferSize = TextField::MaxBufferSize();
  virtual KDCoordinate inputViewHeight() const;
  KDCoordinate m_inputViewMemoizedHeight;
};

}

#endif
