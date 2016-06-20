#include <quiz.h>
#include <kandinsky.h>
#include <assert.h>

QUIZ_CASE(kandinsky_rect_intersect) {
  KDRect a = {
    .x = -5, .y = -5,
    .width = 10, .height = 10
  };
  KDRect b = {
    .x = 0, .y = 0,
    .width = 10, .height = 10
  };
  assert(KDRectIntersect(a,b));
  assert(KDRectIntersect(b,a));
  KDRect c = KDRectIntersection(a,b);
  assert(c.x == 0);
  assert(c.y == 0);
  assert(c.width == 5);
  assert(c.height == 5);
  c = KDRectIntersection(b,a);
  assert(c.x == 0);
  assert(c.y == 0);
  assert(c.width == 5);
  assert(c.height == 5);
}

QUIZ_CASE(kandinsky_rect_union) {
  KDRect a = {
    .x = -5, .y = -5,
    .width = 10, .height = 10
  };
  KDRect b = {
    .x = 0, .y = 0,
    .width = 10, .height = 10
  };

  KDRect c = KDRectUnion(a,b);
  assert(c.x == -5);
  assert(c.y == -5);
  assert(c.width == 15);
  assert(c.height == 15);

  c = KDRectUnion(a,b);
  assert(c.x == -5);
  assert(c.y == -5);
  assert(c.width == 15);
  assert(c.height == 15);
}

QUIZ_CASE(kandinsky_rect_empty_union) {
  KDRect a = {
    .x = 1, .y = 2,
    .width = 3, .height = 4
  };
  KDRect b = {
    .x = 5, .y = 6,
    .width = 0, .height = 0
  };
  KDRect c = {
    .x = -2, .y = -1,
    .width = 0, .height = 1
  };

  KDRect t = KDRectUnion(a,b);
  assert(t.x == a.x);
  assert(t.y == a.y);
  assert(t.width == a.width);
  assert(t.height == a.height);

  t = KDRectUnion(b,a);
  assert(t.x == a.x);
  assert(t.y == a.y);
  assert(t.width == a.width);
  assert(t.height == a.height);

  t = KDRectUnion(a,c);
  assert(t.x == a.x);
  assert(t.y == c.y);
  assert(t.width == a.width);
  assert(t.height == 7);
}
