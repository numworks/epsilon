#ifndef ESCHER_TEXT_VIEW_H
#define ESCHER_TEXT_VIEW_H

#include <escher/childless_view.h>

class TextView : public ChildlessView {
public:
  TextView(const char * text);
  void drawRect(KDRect rect) override;
private:
  const char * m_text;
};

#endif
