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

QUIZ_CASE(kandinsky_rect_difference) {
  KDRect a(-1, 0, 11, 2);
  KDRect b(-4, -3, 4, 7);
  KDRect c(3, -2, 1, 5);
  KDRect d(7, -3, 5, 7);
  KDRect e(-2, -1, 13, 5);
  KDRect f(2, -4, 3, 3);

  KDRect t = e.differencedWith(a);
  assert(t == e);

  t = a.differencedWith(e);
  assert(t == KDRectZero);

  t = f.differencedWith(d);
  assert(t == f);

  t = f.differencedWith(e);
  assert(t == f);

  t = b.differencedWith(e);
  assert(t == b);

  t = c.differencedWith(f);
  assert(t == KDRect(3, -1, 1, 4));

  t = c.differencedWith(a);
  assert(t == c);

  t = c.differencedWith(e);
  assert(t == KDRect(3, -2, 1, 1));

  t = a.differencedWith(b);
  assert(t == KDRect(0, 0, 10, 2));

  t = a.differencedWith(c);
  assert(t == a);

  t = a.differencedWith(d);
  assert(t == KDRect(-1, 0, 8, 2));
}
