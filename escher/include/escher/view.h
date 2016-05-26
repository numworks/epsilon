#ifndef ESCHER_VIEW_H
#define ESCHER_VIEW_H

extern "C" {
#include <stdint.h>
#include <kandinsky.h>
}

/* Key concepts
 * - A View always clips: you cannot draw outside its frame (TODO!)
 * - A View can redraw its whole hierarchy, but a very important optimization is
 *   for it not to do this all the time. So a View will try to track which parts
 *   of it really need to be redrawn.
 * - A view can be offscreen. Until it's attached to the screen, it shouldn't
 *   send any display command.
 */

class View {
public:
  View();

  virtual void drawRect(KDRect rect); // To be implemented. Draw ourself.

  //void addSubview(View * subview);
  //void removeFromSuperview();
  void setFrame(KDRect frame);
  void redraw();

  void setSubview(View * v, int index);
  KDRect bounds();
protected:
  virtual bool isOnScreen();
  virtual int numberOfSubviews() = 0;
  virtual View * subview(int index) = 0;
  virtual void storeSubviewAtIndex(View * v, int index) = 0;
  virtual void layoutSubviews() = 0;
private:
  void redraw(KDRect rect);
  KDRect absoluteDrawingArea();

  View * m_superview;
  KDRect m_frame;
  //TODO: We may want a dynamic size at some point
  /*
  static constexpr uint8_t k_maxNumberOfSubviews = 4;
  View * m_subviews[k_maxNumberOfSubviews];
  */
};

#endif
