#ifndef ESCHER_SOLID_TEXT_AREA_H
#define ESCHER_SOLID_TEXT_AREA_H

#include <escher/text_area.h>

/* SolidTextArea is a text area that draws text in a single solid color over a
 * solid background. */

class SolidTextArea : public TextArea {
public:
  SolidTextArea(Responder * parentResponder, KDText::FontSize fontSize = KDText::FontSize::Large,
    KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite) :
    TextArea(parentResponder, &m_contentView, fontSize),
    m_contentView(fontSize, textColor, backgroundColor) {}
protected:
  class ContentView : public TextArea::ContentView {
  public:
    ContentView(KDText::FontSize fontSize, KDColor textColor, KDColor backgroundColor) :
      TextArea::ContentView(fontSize),
      m_textColor(textColor),
      m_backgroundColor(backgroundColor)
    {
    }
    void drawRect(KDContext * ctx, KDRect rect) const override;
  private:
    KDColor m_textColor;
    KDColor m_backgroundColor;
  };

private:
  const ContentView * nonEditableContentView() const override { return &m_contentView; }
  ContentView m_contentView;
};

#endif
