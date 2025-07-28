#ifndef ESCHER_POINTER_TEXT_VIEW_H
#define ESCHER_POINTER_TEXT_VIEW_H

#include <escher/i18n.h>
#include <escher/text_view.h>

namespace Escher {

class PointerTextView : public TextView {
 public:
  PointerTextView(const char* text = nullptr, KDGlyph::Format format = {});
  const char* text() const override { return m_text; }
  void setText(const char* text) override;

 private:
  const char* m_text;
};

}  // namespace Escher
#endif
