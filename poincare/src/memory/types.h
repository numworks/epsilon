// Deliberate absence of header guard to allow several includes

// Helper to return struct names such as AbsLayoutNode
#define NODE_NAME__(F) F##Node
#define NODE_NAME_(F) NODE_NAME__(F)
#define NODE_NAME(F) NODE_NAME_(SCOPED_NODE(F))

/* Boilerplate to alias NODE(F) as NODE3(F, 0, 0), NODE3 is the varying macro.
 * The first VA_ARGS will push the other arguments of GET4TH allowing it to
 * select the suitable NODE_X macro and call it with the second VA_ARGS list.
 * With 2 args, we have GET4TH(A, B, NODE3, NODE2)(A, B) => NODE2(A, B) */
#define GET4TH(A, B, C, D, ...) D
#define NODE1(F) NODE_USE(F, 0, 0)
#define NODE2(F, N) NODE_USE(F, N, 0)
#define NODE3(F, N, S) NODE_DECL(F, S) NODE_USE(F, N, sizeof(NODE_NAME(F)))
#define NODE(...) GET4TH(__VA_ARGS__, NODE3, NODE2, NODE1)(__VA_ARGS__)

// Macros that may be customized before including this file :

#ifndef RANGE
#define RANGE(NAME, FIRST, LAST)
#endif

#ifndef NODE_USE
#define NODE_USE(NAME, NB_CHILDREN, NODE_SIZE)
#endif

#ifndef NODE_DECL
#define NODE_DECL(NAME, NODE_STRUCT)
#endif

// 1 - Expressions

#ifndef ONLY_LAYOUTS
#define SCOPED_NODE(F) F
#include <poincare/src/expression/types.h>
#undef SCOPED_NODE
#endif

// 2 - Layouts

#define SCOPED_NODE(F) F##Layout
#include <poincare/src/layout/types.h>
#undef SCOPED_NODE

// 3 - Shared types

#ifndef ONLY_LAYOUTS
#define SCOPED_NODE(F) F

NODE(Placeholder, 0, { uint8_t id; })
#if ASSERTIONS
NODE(TreeBorder)
#endif
NODE(NumberOfTypes)

#undef SCOPED_NODE
#endif

#undef RANGE
#undef NODE_USE
#undef NODE_DECL

#undef GET4TH
#undef NODE1
#undef NODE2
#undef NODE3
#undef NODE

#undef ONLY_LAYOUTS
