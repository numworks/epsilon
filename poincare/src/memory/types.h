// Deliberate absence of header guard to allow several includes

// Helper to return struct names such as AbsLayoutNode
#define NODE_NAME__(NAME) NAME##Node
#define NODE_NAME_(NAME) NODE_NAME__(NAME)
#define NODE_NAME(NAME) NODE_NAME_(SCOPED_NODE(NAME))

// 1 - Handle the default arguments

/* Boilerplate to alias NODE(NAME) as NODE3(NAME, 0, 0), NODE3 is the
 * varying macro.  The first VA_ARGS will push the other arguments of
 * GET4TH allowing it to select the suitable NODE_X macro and call it
 * with the second VA_ARGS list.  With 2 args, we have GET4TH(A, B,
 * NODE3, NODE2)(A, B) => NODE2(A, B) */
#define GET5TH(A, B, C, D, E, ...) E
#define NODE1(NAME, FEATURE_SET) NODE_USE_(NAME, FEATURE_SET, 0, 0)
#define NODE2(NAME, FEATURE_SET, NB_CHILDREN) \
  NODE_USE_(NAME, FEATURE_SET, NB_CHILDREN, 0)
#define NODE3(NAME, FEATURE_SET, NB_CHILDREN, NODE_STRUCT) \
  NODE_DECL(NAME, NB_CHILDREN, NODE_STRUCT)                \
  NODE_USE_(NAME, FEATURE_SET, NB_CHILDREN,                \
            sizeof(CustomTypeStructs::NODE_NAME(NAME)))
#define NODE(...) GET5TH(__VA_ARGS__, NODE3, NODE2, NODE1)(__VA_ARGS__)

// 2 - Handle the feature sets

/* The named feature set, for instance MATRIX, with be replaced by the
 * value of the macro variable POINCARE_MATRIX. It needs to resolve to
 * 0 or 1 (it cannot an expression). The functions NODE_USE_0 or
 * NODE_USE_1 will call DISABLED_NODE_USE or NODE_USE to allow for
 * different behaviors when the feature set is enabled or not. */
#define POINCARE_BASE 1

#define NODE_USE_0(NAME, NB_CHILDREN, NODE_STRUCT) \
  DISABLED_NODE_USE(NAME, NB_CHILDREN, NODE_STRUCT)
#define NODE_USE_1(NAME, NB_CHILDREN, NODE_STRUCT) \
  NODE_USE(NAME, NB_CHILDREN, NODE_STRUCT)

#define NODE_USE___(NAME, FEATURE_SET_VALUE, NB_CHILDREN, NODE_STRUCT) \
  NODE_USE_##FEATURE_SET_VALUE(NAME, NB_CHILDREN, NODE_STRUCT)
#define NODE_USE__(NAME, FEATURE_SET_PREFIXED, NB_CHILDREN, NODE_STRUCT) \
  NODE_USE___(NAME, FEATURE_SET_PREFIXED, NB_CHILDREN, NODE_STRUCT)
#define NODE_USE_(NAME, FEATURE_SET, NB_CHILDREN, NODE_STRUCT) \
  NODE_USE__(NAME, POINCARE_##FEATURE_SET, NB_CHILDREN, NODE_STRUCT)

// Macros that may be customized before including this file :

#ifndef RANGE
#define RANGE(NAME, FIRST, LAST)
#endif

#ifndef NODE_USE
#define NODE_USE(NAME, NB_CHILDREN, NODE_SIZE)
#endif

#ifndef NODE_DECL
#define NODE_DECL(NAME, NB_CHILDREN, NODE_STRUCT)
#endif

#ifndef DISABLED_NODE_USE
// By default disabled nodes are doing the same as enabled ones
#define DISABLED_NODE_USE(NAME, NB_CHILDREN, NODE_STRUCT) \
  NODE_USE(NAME, NB_CHILDREN, NODE_STRUCT)
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
#undef NODE_USE
#undef NODE_DECL
#undef DISABLED_NODE_USE

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
