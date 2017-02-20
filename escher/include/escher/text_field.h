#ifndef ESCHER_TEXT_FIELD_H
#define ESCHER_TEXT_FIELD_H

#include <escher/view.h>
#include <escher/responder.h>
#include <escher/text_field_delegate.h>
#include <string.h>

class TextField : public View, public Responder {
public:
  TextField(Responder * parentResponder, char * textBuffer, char * draftTextBuffer, size_t textBufferSize,
    TextFieldDelegate * delegate = nullptr, KDText::FontSize size = KDText::FontSize::Large, float horizontalAlignment = 0.0f,
    float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor = KDColorWhite);
  Toolbox * toolbox() override;
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
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  void reload();
  bool isEditing() const;
  virtual void setEditing(bool isEditing);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  bool m_isEditing;
  char * m_textBuffer;
  char * m_draftTextBuffer;
  size_t m_currentTextLength;
  size_t m_currentCursorLocation;
private:
  void reinitDraftTextBuffer();
  size_t m_textBufferSize;
  TextFieldDelegate * m_delegate;
  float m_horizontalAlignment;
  float m_verticalAlignment;
  KDColor m_textColor;
  KDColor m_backgroundColor;
  KDText::FontSize m_fontSize;
};

#endif
