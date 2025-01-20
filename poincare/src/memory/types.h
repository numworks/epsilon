// Deliberate absence of header guard to allow several includes

// Helper to return struct names such as AbsLayoutNode
#define NODE_NAME__(F) F##Node
#define NODE_NAME_(F) NODE_NAME__(F)
#define NODE_NAME(F) NODE_NAME_(SCOPED_NODE(F))

// 1 - Handle the default arguments

/* Boilerplate to alias NODE(F) as NODE3(F, 0, 0), NODE3 is the varying macro.
 * The first VA_ARGS will push the other arguments of GET4TH allowing it to
 * select the suitable NODE_X macro and call it with the second VA_ARGS list.
 * With 2 args, we have GET4TH(A, B, NODE3, NODE2)(A, B) => NODE2(A, B) */
#define GET5TH(A, B, C, D, E, ...) E
#define NODE1(F, B) NODE_USE_(F, B, 0, 0)
#define NODE2(F, B, N) NODE_USE_(F, B, N, 0)
#define NODE3(F, B, N, S) \
  NODE_DECL(F, N, S) NODE_USE_(F, B, N, sizeof(CustomTypeStructs::NODE_NAME(F)))
#define NODE(...) GET5TH(__VA_ARGS__, NODE3, NODE2, NODE1)(__VA_ARGS__)

// 2 - Handle the features

/* The named feature set, for instance MATRIX, with be replaced by the
 * value of the macro variable POINCARE_MATRIX. It needs to resolve to
 * 0 or 1 (it cannot an expression). The function NODE_USE_0 or
 * NODE_USE_1 will be called accordingly. */
#define POINCARE_BASE 1

#define NODE_USE_0(F, N, S) UNDEF_NODE_USE(F)
#define NODE_USE_1(F, N, S) NODE_USE(F, N, S)

#define NODE_USE___(F, B, N, S) NODE_USE_##B(F, N, S)
#define NODE_USE__(F, B, N, S) NODE_USE___(F, B, N, S)
#define NODE_USE_(F, B, N, S) NODE_USE__(F, POINCARE_##B, N, S)

// Macros that may be customized before including this file :

#ifndef RANGE
#define RANGE(NAME, FIRST, LAST)
#endif

#ifndef UNDEF_RANGE
#define UNDEF_RANGE(NAME, FIRST, LAST)
#endif

#ifndef NODE_USE
#define NODE_USE(NAME, NB_CHILDREN, NODE_SIZE)
#endif

#ifndef NODE_DECL
#define NODE_DECL(NAME, NB_CHILDREN, NODE_STRUCT)
#endif

#ifndef UNDEF_NODE_USE
#define UNDEF_NODE_USE(NAME)
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

NODE(Arbitrary, BASE, NARY, {
  uint16_t size;
  uint8_t data[];
})

NODE(Placeholder, BASE, 0, { uint8_t id; })
#if ASSERTIONS
NODE(TreeBorder, BASE)
#endif
NODE(NumberOfTypes, BASE)

#undef SCOPED_NODE
#endif

#undef RANGE
#undef UNDEF_RANGE
#undef NODE_USE
#undef NODE_DECL
#undef UNDEF_NODE_USE

#undef GET5TH
#undef NODE1
#undef NODE2
#undef NODE3
#undef NODE

#undef POINCARE_BASE
#undef NODE_USE_0
#undef NODE_USE_1
#undef NODE_USE_
#undef NODE_USE__
#undef NODE_USE___

#undef ONLY_LAYOUTS
