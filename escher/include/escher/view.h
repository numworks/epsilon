#ifndef ESCHER_VIEW_H
#define ESCHER_VIEW_H

extern "C" {
#include <stdint.h>
#include <kandinsky.h>
}

#if ESCHER_VIEW_LOGGING
#include <iostream>
#endif

/* Key concepts
 * - A View always clips: you cannot draw outside its frame (TODO!)
 * - A View can redraw its whole hierarchy, but a very important optimization is
 *   for it not to do this all the time. So a View will try to track which parts
 *   of it really need to be redrawn.
 * - A view can be offscreen. Until it's attached to the screen, it shouldn't
 *   send any display command.
 */

class Window;

class View {
  // We only want Window to be able to invoke View::redraw
  friend class Window;
public:
  View();

  virtual void drawRect(KDRect rect) const; // To be implemented. Draw ourself.

  //void addSubview(View * subview);
  //void removeFromSuperview();
  void setFrame(KDRect frame);

  void markAsNeedingRedraw();
  /*
  void markAsDirty() const;
  void redraw() const;
  */

  void setSubview(View * v, int index);
  KDRect bounds() const;
#if ESCHER_VIEW_LOGGING
  friend std::ostream &operator<<(std::ostream &os, View &view);
#endif
protected:
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const;
  virtual void logAttributes(std::ostream &os) const;
#endif
  virtual const Window * window() const;
  virtual int numberOfSubviews() const = 0;
  virtual View * subview(int index) = 0;
  virtual void storeSubviewAtIndex(View * v, int index) = 0;
  virtual void layoutSubviews() = 0;
  View * m_superview;
  KDRect m_frame;
private:
  void redraw(KDRect rect);
  KDPoint absoluteOrigin() const;
  KDRect absoluteDrawingArea() const;

  bool m_needsRedraw;
  //TODO: We may want a dynamic size at some point
  /*
  static constexpr uint8_t k_maxNumberOfSubviews = 4;
  View * m_subviews[k_maxNumberOfSubviews];
  */
};

#endif
