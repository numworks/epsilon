// Deliberate absence of header guard to allow several includes

// 1 - Expressions

#define SCOPED_NODE(F) F
#include <poincare_junior/src/expression/types.h>
#undef SCOPED_NODE

// 2 - Layouts

#define SCOPED_NODE(F) F##Layout
#include <poincare_junior/src/layout/types.h>
#undef SCOPED_NODE

// 3 - Shared types

#define SCOPED_NODE(F) F

/* - Placeholder P
 * | P TAG | ID | */
NODE(Placeholder, 0, 1)
#if ASSERTIONS
NODE(TreeBorder)
#endif
NODE(NumberOfTypes)

#undef SCOPED_NODE
