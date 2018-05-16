#ifndef SETTINGS_HELPERS_H
#define SETTINGS_HELPERS_H

#include <poincare/expression_layout.h>

namespace Settings {
namespace Helpers {

Poincare::ExpressionLayout * CartesianComplexFormat(KDText::FontSize fontSize);
Poincare::ExpressionLayout * PolarComplexFormat(KDText::FontSize fontSize);

}
}

#endif
