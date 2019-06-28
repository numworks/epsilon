#ifndef POINCARE_SIMPLIFICATION_HELPER_H
#define POINCARE_SIMPLIFICATION_HELPER_H

#include <poincare/expression.h>

namespace Poincare {

namespace SimplificationHelper {
  Expression Map(const Expression & e, Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
