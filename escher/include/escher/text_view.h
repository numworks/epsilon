#ifndef ESCHER_TEXT_VIEW_H
#define ESCHER_TEXT_VIEW_H

#include <escher/childless_view.h>

class TextView : public ChildlessView {
public:
  TextView();//;: TextView(nullptr, 0.0f, 0.0f);
  // alignment = 0 -> align left or top
  // alignment = 0.5 -> align center
  // alignment = 1.0 -> align right or bottom
  TextView(const char * text,
      float horizontalAlignment,
      float verticalAlignment);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setText(const char * text);
protected:
#if ESCHER_VIEW_LOGGING
  const char * className() const override;
#endif
private:
  const char * m_text;
  float m_horizontalAlignment;
  float m_verticalAlignment;
};

#endif
