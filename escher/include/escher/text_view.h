#ifndef ESCHER_TEXT_VIEW_H
#define ESCHER_TEXT_VIEW_H

#include <escher/view.h>
#include <kandinsky/color.h>

class TextView : public View {
public:
  // alignment = 0 -> align left or top
  // alignment = 0.5 -> align center
  // alignment = 1.0 -> align right or bottom
  TextView(KDText::FontSize size = KDText::FontSize::Large, float horizontalAlignment = 0.0f, float verticalAlignment = 0.0f,
    KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  KDSize minimalSizeForOptimalDisplay() const override;
  virtual const char * text() const = 0;
  virtual void setText(const char * text) = 0;
  void setFontSize(KDText::FontSize fontSize);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
  KDText::FontSize m_fontSize;
private:
  float m_horizontalAlignment;
  float m_verticalAlignment;
  KDColor m_textColor;
  KDColor m_backgroundColor;
};

#endif
