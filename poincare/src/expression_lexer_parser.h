#include <poincare.h>

/* Usually, YYSTYPE is defined as the union of the objects it might be. Here,
 * some of these objects are non-trivial (specific copy-constructors and
 * destructors), so they cannot be part of a union. We must use a struct to
 * define YYSTYPE. */

struct OurNodeValue{
    Poincare::Expression expression;
    char character;
};
#define YYSTYPE OurNodeValue
