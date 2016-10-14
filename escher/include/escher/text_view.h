#ifndef ESCHER_TEXT_VIEW_H
#define ESCHER_TEXT_VIEW_H

#include <escher/childless_view.h>
#include <kandinsky/color.h>

class TextView : public ChildlessView {
public:
  TextView();//;: TextView(nullptr, 0.0f, 0.0f);
  // alignment = 0 -> align left or top
  // alignment = 0.5 -> align center
  // alignment = 1.0 -> align right or bottom
  TextView(float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  KDSize minimalSizeForOptimalDisplay() override;
protected:
  virtual const char * text() const = 0;
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
private:
  float m_horizontalAlignment;
  float m_verticalAlignment;
  KDColor m_textColor;
  KDColor m_backgroundColor;
};

#endif
