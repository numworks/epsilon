#ifndef SETTINGS_HELPERS_H
#define SETTINGS_HELPERS_H

#include <poincare.h>

namespace Settings {
namespace Helpers {

Poincare::Layout CartesianComplexFormat(KDText::FontSize fontSize);
Poincare::Layout PolarComplexFormat(KDText::FontSize fontSize);

}
}

#endif
