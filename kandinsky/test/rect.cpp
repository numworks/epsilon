#include <assert.h>
#include <kandinsky/rect.h>
#include <quiz.h>

QUIZ_CASE(kandinsky_rect_intersect) {
  KDRect a(-5, -5, 10, 10);
  KDRect b(0, 0, 10, 10);
  quiz_assert(a.intersects(b));
  quiz_assert(b.intersects(a));
  KDRect c = a.intersectedWith(b);
  KDRect result(0, 0, 5, 5);
  quiz_assert(c == result);
  c = b.intersectedWith(a);
  quiz_assert(c == result);
}

QUIZ_CASE(kandinsky_rect_union) {
  KDRect a(-5, -5, 10, 10);
  KDRect b(0, 0, 10, 10);
  KDRect c = a.unionedWith(b);
  quiz_assert(c == KDRect(-5, -5, 15, 15));
}

QUIZ_CASE(kandinsky_rect_empty_union) {
  KDRect a(1, 2, 3, 4);
  KDRect b(5, 6, 0, 0);
  KDRect c(-2, -1, 0, 1);

  KDRect t = a.unionedWith(b);
  quiz_assert(t == a);

  t = b.unionedWith(a);
  quiz_assert(t == a);

  t = a.unionedWith(c);
  quiz_assert(t == a);
}

QUIZ_CASE(kandinsky_rect_difference) {
  KDRect a(-1, 0, 11, 2);
  KDRect b(-4, -3, 4, 7);
  KDRect c(3, -2, 1, 5);
  KDRect d(7, -3, 5, 7);
  KDRect e(-2, -1, 13, 5);
  KDRect f(2, -4, 3, 3);

  KDRect t = e.differencedWith(a);
  quiz_assert(t == e);

  t = a.differencedWith(e);
  quiz_assert(t == KDRectZero);

  t = f.differencedWith(d);
  quiz_assert(t == f);

  t = f.differencedWith(e);
  quiz_assert(t == f);

  t = b.differencedWith(e);
  quiz_assert(t == b);

  t = c.differencedWith(f);
  quiz_assert(t == KDRect(3, -1, 1, 4));

  t = c.differencedWith(a);
  quiz_assert(t == c);

  t = c.differencedWith(e);
  quiz_assert(t == KDRect(3, -2, 1, 1));

  t = a.differencedWith(b);
  quiz_assert(t == KDRect(0, 0, 10, 2));

  t = a.differencedWith(c);
  quiz_assert(t == a);

  t = a.differencedWith(d);
  quiz_assert(t == KDRect(-1, 0, 8, 2));
}
