#ifndef ESCHER_POINTER_TEXT_VIEW_H
#define ESCHER_POINTER_TEXT_VIEW_H

#include <escher/i18n.h>
#include <escher/text_view.h>

namespace Escher {

class PointerTextView : public TextView {
 public:
  PointerTextView(KDFont::Size font = KDFont::Size::Large,
                  const char* text = nullptr, float horizontalAlignment = 0.0f,
                  float verticalAlignment = 0.0f,
                  KDColor textColor = KDColorBlack,
                  KDColor backgroundColor = KDColorWhite);
  const char* text() const override { return m_text; }
  void setText(const char* text) override;

 private:
  const char* m_text;
};

}  // namespace Escher
#endif
