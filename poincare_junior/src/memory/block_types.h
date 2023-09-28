// Deliberate absence of header guard to allow several includes

// 1 - Expressions

#define SCOPED_TYPE(F) F
#include <poincare_junior/src/expression/block_types.h>
#undef SCOPED_TYPE

// 2 - Layouts

#define SCOPED_TYPE(F) F##Layout
#include <poincare_junior/src/layout/block_types.h>
#undef SCOPED_TYPE

// 3 - Shared types

#define SCOPED_TYPE(F) F
TYPE(Placeholder)
TYPE(SystemList)
#if ASSERTIONS
TYPE(TreeBorder)
ALIAS(NumberOfTypes = TreeBorder + 1)
#else
ALIAS(NumberOfTypes = SystemList + 1)
#endif
#undef SCOPED_TYPE
