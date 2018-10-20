/* Usually, YYSTYPE is defined as the union of the objects it might be. Here,
 * Expression is non-trivial (specific copy-constructors and destructors), so
 * it cannot be part of a union. */

#define YYSTYPE Poincare::Expression
