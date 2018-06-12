#ifndef SHARED_BUFFER_TEXT_VIEW_WITH_TEXT_FIELD_H
#define SHARED_BUFFER_TEXT_VIEW_WITH_TEXT_FIELD_H
#include <escher.h>

namespace Shared {

class BufferTextViewWithTextField : public View, public Responder {
public:
  BufferTextViewWithTextField(Responder * parentResponder, TextFieldDelegate * delegate = nullptr, KDText::FontSize size = KDText::FontSize::Large);
  KDSize minimalSizeForOptimalDisplay() const override;
  TextField * textField() { return &m_textField; }
  void setBufferText(const char * text);
  void drawRect(KDContext * ctx, KDRect rect) const override;

  // Responder
  void didBecomeFirstResponder() override;
private:
  constexpr static int k_textFieldBufferSize = TextField::maxBufferSize();
  constexpr static KDCoordinate k_bufferTextWidth = 35;
  constexpr static KDCoordinate k_textFieldVerticalMargin = 3;
  constexpr static KDCoordinate k_borderWidth = 1;
  int numberOfSubviews() const override { return 2; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  KDRect textFieldFrame() const;
  BufferTextView m_bufferTextView;
  TextField m_textField;
  char m_textFieldBuffer[k_textFieldBufferSize];
};

}

#endif
