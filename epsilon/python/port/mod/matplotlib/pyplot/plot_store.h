#ifndef PYTHON_MATPLOTLIB_PLOT_STORE_H
#define PYTHON_MATPLOTLIB_PLOT_STORE_H

// #include <apps/shared/curve_view_range.h>
#include <apps/shared/interactive_curve_view_range.h>
extern "C" {
#include <py/runtime.h>
}

namespace Matplotlib {

class PlotStore : public Shared::InteractiveCurveViewRange {
 public:
  PlotStore();
  void flush();

  // Iterators

  template <class T>
  class ListIterator {
   public:
    static ListIterator Begin(mp_obj_t list);
    static ListIterator End(mp_obj_t list);
    T operator*();
    ListIterator& operator++();
    bool operator!=(const ListIterator& it) const;

   private:
    ListIterator() : m_tupleIndex(0) {}
    mp_obj_t* m_tuples;
    size_t m_numberOfTuples;
    size_t m_tupleIndex;
  };

  template <class T>
  class Iterable {
   public:
    Iterable(mp_obj_t list) : m_list(list) {}
    T begin() const { return T::Begin(m_list); }
    T end() const { return T::End(m_list); }

   private:
    mp_obj_t m_list;
  };

  // Dot

  class Dot {
   public:
    Dot(mp_obj_t tuple);
    float x() const { return m_x; }
    float y() const { return m_y; }
    KDColor color() const { return m_color; }

   private:
    float m_x;
    float m_y;
    KDColor m_color;
  };

  void addDot(mp_obj_t x, mp_obj_t y, KDColor c);
  Iterable<ListIterator<Dot>> dots() {
    return Iterable<ListIterator<Dot>>(m_dots);
  }

  // Segment

  class Segment {
   public:
    Segment(mp_obj_t tuple);
    float xStart() const { return m_xStart; }
    float yStart() const { return m_yStart; }
    float xEnd() const { return m_xEnd; }
    float yEnd() const { return m_yEnd; }
    float arrowWidth() const { return m_arrowWidth; }
    KDColor color() const { return m_color; }

   private:
    float m_xStart;
    float m_yStart;
    float m_xEnd;
    float m_yEnd;
    float m_arrowWidth;
    KDColor m_color;
  };

  void addSegment(mp_obj_t xStart, mp_obj_t yStart, mp_obj_t xEnd,
                  mp_obj_t yEnd, KDColor c,
                  mp_obj_t arrowWidth = mp_obj_new_float(NAN));
  Iterable<ListIterator<Segment>> segments() {
    return Iterable<ListIterator<Segment>>(m_segments);
  }

  // Rect

  class Rect {
   public:
    Rect(mp_obj_t tuple);
    float left() const { return m_left; }
    float right() const { return m_right; }
    float top() const { return m_top; }
    float bottom() const { return m_bottom; }
    KDColor color() const { return m_color; }

   private:
    float m_left;
    float m_right;
    float m_top;
    float m_bottom;
    KDColor m_color;
  };

  void addRect(mp_obj_t x, mp_obj_t y, mp_obj_t width, mp_obj_t height,
               KDColor c);
  Iterable<ListIterator<Rect>> rects() {
    return Iterable<ListIterator<Rect>>(m_rects);
  }

  // Label

  class Label {
   public:
    Label(mp_obj_t tuple);
    float x() const { return m_x; }
    float y() const { return m_y; }
    const char* string() const { return m_string; }

   private:
    float m_x;
    float m_y;
    const char* m_string;
  };

  void addLabel(mp_obj_t x, mp_obj_t y, mp_obj_t string);
  Iterable<ListIterator<Label>> labels() {
    return Iterable<ListIterator<Label>>(m_labels);
  }

  void setAxesRequested(bool b) { m_axesRequested = b; }
  bool axesRequested() const { return m_axesRequested; }
  void setAxesAuto(bool b) { m_axesAuto = b; }
  void setShow(bool b) { m_show = b; }
  bool show() { return m_show; }
  void initRange();

  void setGridRequested(bool b) { m_gridRequested = b; }
  bool gridRequested() const { return m_gridRequested; }

 private:
  mp_obj_t m_dots;      // List of (x, y, color)
  mp_obj_t m_labels;    // List of (x, y, string)
  mp_obj_t m_segments;  // List of (x, y, dx, dy, style, color)
  mp_obj_t m_rects;     // List of (x, y, w, h, color)
  bool m_axesRequested;
  bool m_axesAuto;
  bool m_gridRequested;
  bool m_show;
};

}  // namespace Matplotlib

#endif
