// Shared types
TYPE(Placeholder)
TYPE(SystemList)
#if ASSERTIONS
TYPE(TreeBorder)
ALIAS(NumberOfTypes = TreeBorder + 1)
#else
ALIAS(NumberOfTypes = SystemList + 1)
#endif
