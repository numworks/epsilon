#ifndef ESCHER_TEXT_FIELD_H
#define ESCHER_TEXT_FIELD_H

#include <escher/view.h>
#include <escher/responder.h>
#include <escher/text_field_delegate.h>
#include <string.h>

class TextField : public View, public Responder {
public:
  TextField(Responder * parentResponder, char * textBuffer, size_t textBufferSize, TextFieldDelegate * delegate = nullptr);
  // View
  void drawRect(KDContext * ctx, KDRect rect) const override;
  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  const char * text() const;
  int textLength() const;
  void setText(const char * text);
  void setTextFieldDelegate(TextFieldDelegate * delegate);
  /* If the text to be appended is too long to be added without overflowing the
   * buffer, nothing is done (not even adding few letters from the text to reach
   * the maximum buffer capacity. */
  int cursorLocation() const;
  void setCursorLocation(int location);
  void insertTextAtLocation(const char * text, int location);
  KDSize minimalSizeForOptimalDisplay() override;
protected:
  void reload();
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  char * m_textBuffer;
  size_t m_currentTextLength;
  size_t m_currentCursorLocation;
private:
  size_t m_textBufferSize;
  TextFieldDelegate * m_delegate;
};

#endif
