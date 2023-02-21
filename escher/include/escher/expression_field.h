#ifndef ESCHER_EXPRESSION_FIELD_H
#define ESCHER_EXPRESSION_FIELD_H

#include <escher/layout_field.h>
#include <escher/layout_field_delegate.h>
#include <escher/metric.h>
#include <poincare/layout.h>

namespace Escher {

class ExpressionField : public LayoutField {
 public:
  ExpressionField(Responder* parentResponder,
                  InputEventHandlerDelegate* inputEventHandler,
                  LayoutFieldDelegate* layoutFieldDelegate,
                  float horizontalAlignment = KDContext::k_alignLeft,
                  float verticalAlignment = KDContext::k_alignCenter);

  void clearAndSetEditing(bool isEditing);
  /* Warning: this function is VERY dangerous! Indeed: sometimes the
   * m_layoutField might overflow the m_textBuffer once serialized
   * and still have been accepted before because the model can hold a longer
   * buffer. This is the case in the application 'Calculation' and we do not
   * use text() there... TODO: change text() for fillTextInBuffer?*/
  const char* text();
  void setText(const char* text);
  bool inputViewHeightDidChange();
  void reload();
  void restoreContent(const char* buffer, size_t size, int* cursorOffset,
                      int* position);
  void setTextEditionBuffer(char* buffer, size_t bufferSize) {
    m_draftBuffer = buffer;
    m_draftBufferSize = bufferSize;
  }

  /* View */
  KDSize minimalSizeForOptimalDisplay() const override;

  /* Responder */
  void didBecomeFirstResponder() override;

  virtual KDCoordinate inputViewHeight() const;
  using LayoutField::layoutSubviews;

 private:
  constexpr static KDCoordinate k_minimalHeight =
      KDFont::GlyphHeight(KDFont::Size::Large);
  KDCoordinate m_inputViewMemoizedHeight;
  char* m_draftBuffer;
  size_t m_draftBufferSize;
};

}  // namespace Escher

#endif
