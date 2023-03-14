#ifndef ESCHER_VIEW_H
#define ESCHER_VIEW_H

#include <kandinsky/context.h>
#include <kandinsky/point.h>
#include <kandinsky/rect.h>
#include <kandinsky/size.h>

extern "C" {
#include <stdint.h>
}

#if ESCHER_VIEW_LOGGING
#include <iostream>
#endif

namespace Shared {
class MemoizedCursorView;
}

namespace Escher {

/* Key concepts
 * - A View always clips: you cannot draw outside its frame
 * - A View can redraw its whole hierarchy, but a very important optimization is
 *   for it not to do this all the time. So a View will try to track which parts
 *   of it really need to be redrawn.
 * - A view can be offscreen. Until it's attached to the screen, it shouldn't
 *   send any display command. */

class Window;

class View {
  friend class Shared::MemoizedCursorView;
  friend class ModalViewController;
  friend class TextCursorView;
  friend class TransparentView;
  // We only want Window to be able to invoke View::redraw
  friend class Window;

 public:
  View() : m_frame(KDRectZero), m_dirtyRect(KDRectZero) {}

  /* The drawRect method should be implemented by each View subclass. In a
   * typical drawRect implementation, a subclass will make drawing calls to the
   * Kandinsky library using the provided context. */
  virtual void drawRect(KDContext *ctx, KDRect rect) const {
    // By default, a view doesn't do anything, it's transparent
  }

  void setSize(KDSize size);
  void setFrame(KDRect frame, bool force);
  void setAbsoluteFrame(KDRect frame, bool force) { m_frame = frame; }
  void setChildFrame(View *child, KDRect frame, bool force);
  KDPoint pointFromPointInView(View *view, KDPoint point);
  KDRect absoluteFrame() const { return m_frame; }
  KDPoint absoluteOrigin() const { return m_frame.origin(); }

  KDRect bounds() const;
  KDRect dirtyRect() const { return m_dirtyRect; }
  virtual bool isVisible() const { return true; }

  virtual View *subview(int index);

  virtual KDSize minimalSizeForOptimalDisplay() const { return KDSizeZero; }

#if ESCHER_VIEW_LOGGING
  friend std::ostream &operator<<(std::ostream &os, View &view);
  __attribute__((__used__)) void log() const;
#endif
 protected:
  /* The whole point of the dirty-tracking mechanism is to identify which
   * pixels have to be redrawn. So in the end it doesn't really need to be bound
   * to a view, it's really absolute pixels that count.
   *
   * That being said, what are the case of dirtyness that we know of?
   *  - Scrolling -> well, everything has to be redrawn anyway
   *  - Moving a cursor -> In that case, there's really a much more efficient
   * way
   *  - ... and that's all I can think of.
   */
  virtual void markRectAsDirty(KDRect rect);
  void markAbsoluteRectAsDirty(KDRect rect);
#if ESCHER_VIEW_LOGGING
  virtual const char *className() const;
  virtual void logAttributes(std::ostream &os) const;
#endif
  virtual int numberOfSubviews() const { return 0; }
  virtual View *subviewAtIndex(int index) { return nullptr; }

 private:
  virtual void layoutSubviews(bool force = false) {}
  void translate(KDPoint origin);
  KDRect redraw(KDRect rect, KDRect forceRedrawRect = KDRectZero);

  /* At destruction, subviews aren't notified that their own pointer
   * 'm_superview' is outdated. This is not an issue since all view hierarchy
   * is created or destroyed at once: when the app is packed or unpacked. The
   * view and its subviews are then destroyed concomitantly.
   * Otherwise, we would just have to implement the destructor to notify
   * subviews that 'm_superview = nullptr'. */
  KDRect m_frame;      // absolute
  KDRect m_dirtyRect;  // absolute
};

}  // namespace Escher
#endif
