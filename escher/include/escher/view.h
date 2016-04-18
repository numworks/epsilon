#ifndef ESCHER_VIEW_H
#define ESCHER_VIEW_H

extern "C" {
#include <stdint.h>
#include <kandinsky.h>
}

class View {
public:
  View(KDRect frame);
  ~View();
  void draw();
  virtual void drawRect(KDRect rect);
  void addSubview(View * subview);
  void removeFromSuperview();
  void setFrame(KDRect frame);
protected:
  KDRect bounds();
private:
  void setOriginAndDrawRect(KDRect rect);
  KDPoint absoluteOrigin();
  //TODO: We may want a dynamic size at some point
  static constexpr uint8_t k_maxNumberOfSubviews = 4;
  KDRect m_frame;
  View * m_superview;
  View * m_subviews[k_maxNumberOfSubviews];
  uint8_t m_numberOfSubviews;
};

#endif
