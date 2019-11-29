#ifndef ESCHER_SOLID_TEXT_AREA_H
#define ESCHER_SOLID_TEXT_AREA_H

#include <escher/text_area.h>

/* SolidTextArea is a text area that draws text in a single solid color over a
 * solid background. */

class SolidTextArea : public TextArea {
public:
  SolidTextArea(Responder * parentResponder, const KDFont * font = KDFont::LargeFont,
    KDColor textColor = Palette::PrimaryText, KDColor backgroundColor = Palette::BackgroundHard) :
    TextArea(parentResponder, &m_contentView, font),
    m_contentView(font, textColor, backgroundColor) {}
protected:
  class ContentView : public TextArea::ContentView {
  public:
    ContentView(const KDFont * font, KDColor textColor, KDColor backgroundColor) :
      TextArea::ContentView(font),
      m_textColor(textColor),
      m_backgroundColor(backgroundColor)
    {
    }
    void clearRect(KDContext * ctx, KDRect rect) const override;
    void drawLine(KDContext * ctx, int line, const char * text, size_t length, int fromColumn, int toColumn) const override;
  private:
    KDColor m_textColor;
    KDColor m_backgroundColor;
  };

private:
  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;
};

#endif
