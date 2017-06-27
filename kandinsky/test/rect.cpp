#include <quiz.h>
#include <kandinsky.h>
#include <assert.h>

QUIZ_CASE(kandinsky_rect_intersect) {
  KDRect a(-5,-5, 10, 10);
  KDRect b(0, 0, 10, 10);
  assert(a.intersects(b));
  assert(b.intersects(a));
  KDRect c = a.intersectedWith(b);
  assert(c.x() == 0);
  assert(c.y() == 0);
  assert(c.width() == 5);
  assert(c.height() == 5);
  c = b.intersectedWith(a);
  assert(c.x() == 0);
  assert(c.y() == 0);
  assert(c.width() == 5);
  assert(c.height() == 5);
}

QUIZ_CASE(kandinsky_rect_union) {
  KDRect a(-5, -5, 10, 10);
  KDRect b(0, 0, 10, 10);
  KDRect c = a.unionedWith(b);
  assert(c.x() == -5);
  assert(c.y() == -5);
  assert(c.width() == 15);
  assert(c.height() == 15);

  c = a.unionedWith(b);
  assert(c.x() == -5);
  assert(c.y() == -5);
  assert(c.width() == 15);
  assert(c.height() == 15);
}

QUIZ_CASE(kandinsky_rect_empty_union) {
  KDRect a(1, 2, 3, 4);
  KDRect b(5, 6, 0, 0);
  KDRect c(-2, -1, 0, 1);

  KDRect t = a.unionedWith(b);
  assert(t.x() == a.x());
  assert(t.y() == a.y());
  assert(t.width() == a.width());
  assert(t.height() == a.height());

  t = b.unionedWith(a);
  assert(t.x() == a.x());
  assert(t.y() == a.y());
  assert(t.width() == a.width());
  assert(t.height() == a.height());

  t = a.unionedWith(c);
  assert(t.x() == a.x());
  assert(t.y() == a.y());
  assert(t.width() == a.width());
  assert(t.height() == a.height());
}
