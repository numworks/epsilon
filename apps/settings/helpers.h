#ifndef SETTINGS_HELPERS_H
#define SETTINGS_HELPERS_H

#include <poincare.h>

namespace Settings {
namespace Helpers {

Poincare::LayoutReference CartesianComplexFormat(KDText::FontSize fontSize);
Poincare::LayoutReference PolarComplexFormat(KDText::FontSize fontSize);

}
}

#endif
