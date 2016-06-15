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
