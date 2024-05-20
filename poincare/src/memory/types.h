// Deliberate absence of header guard to allow several includes

// Macros that may be customized before including this file :

#ifndef RANGE
#define RANGE(NAME, FIRST, LAST)
#endif

#ifndef NODE3
#define NODE3(NAME, NB_CHILDREN, NODE_STRUCT)
#endif

// 1 - Expressions

#define SCOPED_NODE(F) F
#include <poincare/src/expression/types.h>
#undef SCOPED_NODE

// 2 - Layouts

#define SCOPED_NODE(F) F##Layout
#include <poincare/src/layout/types.h>
#undef SCOPED_NODE

// 3 - Shared types

#define SCOPED_NODE(F) F

NODE(Placeholder, 0, { uint8_t id; })
#if ASSERTIONS
NODE(TreeBorder)
#endif
NODE(NumberOfTypes)

#undef SCOPED_NODE

#undef RANGE
#undef NODE3
