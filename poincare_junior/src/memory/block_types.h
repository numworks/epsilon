// Deliberate absence of header guard to allow several includes

// 1 - Expressions

#include <poincare_junior/src/expression/block_types.h>

// 2 - Layouts

#include <poincare_junior/src/layout/block_types.h>

// 3 - Shared types

TYPE(Placeholder)
TYPE(SystemList)
#if ASSERTIONS
TYPE(TreeBorder)
ALIAS(NumberOfTypes = TreeBorder + 1)
#else
ALIAS(NumberOfTypes = SystemList + 1)
#endif
