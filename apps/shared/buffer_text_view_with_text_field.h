#ifndef SHARED_BUFFER_TEXT_VIEW_WITH_TEXT_FIELD_H
#define SHARED_BUFFER_TEXT_VIEW_WITH_TEXT_FIELD_H
#include <escher.h>

namespace Shared {

class BufferTextViewWithTextField : public View, public Responder {
public:
  constexpr static KDCoordinate k_height = 50; //TODO
  BufferTextViewWithTextField(Responder * parentResponder, TextFieldDelegate * delegate = nullptr, KDText::FontSize size = KDText::FontSize::Large);
  KDSize minimalSizeForOptimalDisplay() const override;
  void setBufferText(const char * text);
  void setTextFieldText(const char * text);

  // Responder
  void didBecomeFirstResponder() override;
private:
  constexpr static int k_textFieldBufferSize = TextField::maxBufferSize();
  constexpr static KDCoordinate k_bufferTextWidth = 70; //TODO
  int numberOfSubviews() const override { return 2; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  BufferTextView m_bufferTextView;
  TextField m_textField;
  char m_textFieldBuffer[k_textFieldBufferSize];
};

}

#endif
