#ifndef ESCHER_TEXT_VIEW_H
#define ESCHER_TEXT_VIEW_H

#include <escher/view.h>

class TextView : public View {
public:
  TextView(KDPoint origin, const char * text);
  void drawRect(KDRect rect) override;
private:
  const char * m_text;
};

#endif
