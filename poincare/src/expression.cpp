#include <poincare/expression.h>
#include <string.h>

void Expression::forEachChild(ExpressionAction action) {
  Expression ** childPointer = children();
  if (childPointer == NULL) {
    return;
  }
  while(*childPointer != NULL) {
    (*childPointer->*action)();
    childPointer++;
  }
}

void Expression::recursiveLayout() {
  forEachChild(&Expression::recursiveLayout);
  layout();
}

void Expression::recursiveDraw() {
  KDPoint origin = KDGetOrigin();
  KDSetOrigin(KDPOINT(origin.x + m_frame.origin.x,
        origin.y + m_frame.origin.y));

  forEachChild(&Expression::recursiveDraw);

  draw();

  KDSetOrigin(origin);
}

void Expression::draw() {
  // No-op by default
}

void Expression::layout() {
  // No-op by default
}
