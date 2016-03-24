#ifndef POINCARE_SIMPLIFY_ADDITION_MERGE_H
#define POINCARE_SIMPLIFY_ADDITION_MERGE_H

#include "simplify.h"

/*
 *   +                  +
 *  / \               / | \
 * a   +         ->  a  c  d
 *    / \
 *   c   d
 */

Expression * SimplifyAdditionMerge(Expression * e);

#endif
