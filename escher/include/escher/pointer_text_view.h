#ifndef ESCHER_POINTER_TEXT_VIEW_H
#define ESCHER_POINTER_TEXT_VIEW_H

#include <escher/text_view.h>

class PointerTextView : public TextView {
public:
  PointerTextView();
  PointerTextView(const char * text, float horizontalAlignment, float verticalAlignment,
    KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  void setText(const char * text);
protected:
  const char * text() const override;
private:
  const char * m_textPointer;
};

#endif