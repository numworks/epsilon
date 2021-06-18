#ifndef ESCHER_POINTER_TEXT_VIEW_H
#define ESCHER_POINTER_TEXT_VIEW_H

#include <escher/text_view.h>
#include <escher/i18n.h>

class PointerTextView : public TextView {
public:
  PointerTextView(const KDFont * font = KDFont::LargeFont, const char * text = nullptr, float horizontalAlignment = 0.0f, float verticalAlignment = 0.0f,
    KDColor textColor = Palette::PrimaryText, KDColor backgroundColor = Palette::BackgroundHard);
  const char * text() const override { return m_text; }
  void setText(const char * text) override;
private:
  const char * m_text;
};

#endif
