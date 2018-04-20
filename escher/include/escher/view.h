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
 * - A View always clips: you cannot draw outside its frame
 * - A View can redraw its whole hierarchy, but a very important optimization is
 *   for it not to do this all the time. So a View will try to track which parts
 *   of it really need to be redrawn.
 * - A view can be offscreen. Until it's attached to the screen, it shouldn't
 *   send any display command. */

class Window;

class View {
  // We only want Window to be able to invoke View::redraw
  friend class Window;
public:
  View();
  virtual ~View();
  View(const View& other) = delete;
  View(View&& other) = delete;
  View& operator=(const View& other) = delete;
  View& operator=(View&& other) = delete;
  void resetSuperview();
  /* The drawRect method should be implemented by each View subclass. In a
   * typical drawRect implementation, a subclass will make drawing calls to the
   * Kandinsky library using the provided context. */
  virtual void drawRect(KDContext * ctx, KDRect rect) const;

  void setSize(KDSize size);
  void setFrame(KDRect frame);
  KDPoint pointFromPointInView(View * view, KDPoint point);

  KDRect bounds() const;
  View * subview(int index);

  virtual KDSize minimalSizeForOptimalDisplay() const;

#if ESCHER_VIEW_LOGGING
  friend std::ostream &operator<<(std::ostream &os, View &view);
#endif
protected:
  /* The whole point of the dirty-tracking mechanism is to identify which
   * pixels have to be redrawn. So in the end it doesn't really need to be bound
   * to a view, it's really absolute pixels that count.
   *
   * That being said, what are the case of dirtyness that we know of?
   *  - Scrolling -> well, everything has to be redrawn anyway
   *  - Moving a cursor -> In that case, there's really a much more efficient way
   *  - ... and that's all I can think of.
   */
  void markRectAsDirty(KDRect rect);
#if ESCHER_VIEW_LOGGING
  virtual const char * className() const;
  virtual void logAttributes(std::ostream &os) const;
#endif
  KDRect m_frame;
private:
  virtual int numberOfSubviews() const;
  virtual View * subviewAtIndex(int index);
  virtual void layoutSubviews();
  virtual const Window * window() const;
  KDRect redraw(KDRect rect, KDRect forceRedrawRect = KDRectZero);
  KDPoint absoluteOrigin() const;
  KDRect absoluteVisibleFrame() const;

  View * m_superview;
  KDRect m_dirtyRect;
};

#endif
