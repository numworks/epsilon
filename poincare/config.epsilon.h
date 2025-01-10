#ifndef POINCARE_CONFIG_H
#define POINCARE_CONFIG_H

///// General configuration

#define POINCARE_TREE_STACK_SIZE (1024 * 16)

/////  Block subsets

/* The following options control the set of active block
 * types. Related builtins and layouts types are enabled
 * accordingly. */

// User sequence node
#define POINCARE_SEQUENCE 1

#define POINCARE_SUM_AND_PRODUCT 1
#define POINCARE_DIFF 1
#define POINCARE_INTEGRAL 1

// Matrix and matrix operations blocks
#define POINCARE_MATRIX 1

// List related operations
// Nb: the List block is always present as an helper
#define POINCARE_LIST 1

#define POINCARE_POINT 1

// Unit node and representatives
#define POINCARE_UNIT 1

// True, False, comparisons and logic operators
#define POINCARE_BOOLEAN 1

#define POINCARE_PIECEWISE POINCARE_BOOLEAN

#define POINCARE_DISTRIBUTION 1

#define POINCARE_PT_COMBINATORICS_LAYOUTS 1

#endif
