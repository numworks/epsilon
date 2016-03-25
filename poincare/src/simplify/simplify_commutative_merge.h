#ifndef POINCARE_SIMPLIFY_COMMUTATIVE_MERGE_H
#define POINCARE_SIMPLIFY_COMMUTATIVE_MERGE_H

#include "simplify.h"

/*
 *   +                  +
 *  / \               / | \
 * a   +         ->  a  c  d
 *    / \
 *   c   d
 */

Expression * SimplifyCommutativeMerge(Expression * e);

#endif
