// Deliberate absence of header guard to allow several includes

// 1 - Expressions

#define SCOPED_TYPE(F) F
#include <poincare_junior/src/expression/types.h>
#undef SCOPED_TYPE

// 2 - Layouts

#define SCOPED_TYPE(F) F##Layout
#include <poincare_junior/src/layout/types.h>
#undef SCOPED_TYPE

// 3 - Shared types

#define SCOPED_TYPE(F) F
TYPE(Placeholder, 0)
TYPE(SystemList, NARY)
#if ASSERTIONS
TYPE(TreeBorder, 0)
#endif
TYPE(NumberOfTypes, 0)
#undef SCOPED_TYPE
