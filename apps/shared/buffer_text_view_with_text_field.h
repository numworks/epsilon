#ifndef SHARED_BUFFER_TEXT_VIEW_WITH_TEXT_FIELD_H
#define SHARED_BUFFER_TEXT_VIEW_WITH_TEXT_FIELD_H
#include <escher.h>

namespace Shared {

class BufferTextViewWithTextField : public Escher::View, public Escher::Responder {
public:
  BufferTextViewWithTextField(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::TextFieldDelegate * delegate = nullptr, const KDFont * font = KDFont::LargeFont);
  KDSize minimalSizeForOptimalDisplay() const override;
  Escher::TextField * textField() { return &m_textField; }
  void setBufferText(const char * text);
  void drawRect(KDContext * ctx, KDRect rect) const override;

  // Responder
  void didBecomeFirstResponder() override;
private:
  constexpr static KDCoordinate k_bufferTextWidth = 35;
  constexpr static KDCoordinate k_textFieldVerticalMargin = 3;
  constexpr static KDCoordinate k_borderWidth = 1;
  int numberOfSubviews() const override { return 2; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  KDRect textFieldFrame() const;
  Escher::BufferTextView m_bufferTextView;
  Escher::TextField m_textField;
};

}

#endif
